// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#include "sw/device/lib/dif/dif_otbn.h"
#include "sw/device/lib/runtime/ibex.h"
#include "sw/device/lib/runtime/log.h"
#include "sw/device/lib/runtime/otbn.h"
#include "sw/device/lib/base/memory.h"
#include "sw/device/lib/runtime/log.h"
#include "sw/device/sca/lib/prng.h"
#include "sw/device/sca/lib/sca.h"
#include "sw/device/sca/lib/simple_serial.h"

#include "hw/top_earlgrey/sw/autogen/top_earlgrey.h"


#include "sw/device/lib/base/mmio.h"
#include "sw/device/lib/dif/dif_csrng.h"
#include "sw/device/lib/dif/dif_entropy_src.h"
#include "edn_regs.h"  // Generated
#include "hw/top_earlgrey/sw/autogen/top_earlgrey.h"


static void setup_entropy_src(void) {
  dif_entropy_src_t entropy_src;
  CHECK_DIF_OK(dif_entropy_src_init(
      mmio_region_from_addr(TOP_EARLGREY_ENTROPY_SRC_BASE_ADDR), &entropy_src));

  // Disable entropy for test purpose, as it has been turned on by ROM
  CHECK_DIF_OK(dif_entropy_src_disable(&entropy_src));

  const dif_entropy_src_config_t config = {
      .mode = kDifEntropySrcModePtrng,
      .tests =
          {
              [kDifEntropySrcTestRepCount] = false,
              [kDifEntropySrcTestAdaptiveProportion] = false,
              [kDifEntropySrcTestBucket] = false,
              [kDifEntropySrcTestMarkov] = false,
              [kDifEntropySrcTestMailbox] = false,
              [kDifEntropySrcTestVendorSpecific] = false,
          },
      // this field needs to manually toggled by software.  Disable for now
      .reset_health_test_registers = false,
      .single_bit_mode = kDifEntropySrcSingleBitModeDisabled,
      .route_to_firmware = false,
      .fw_override = {
          .enable = false,
          .entropy_insert_enable = false,
          .buffer_threshold = kDifEntropyFifoIntDefaultThreshold,
      }};
  CHECK_DIF_OK(dif_entropy_src_configure(&entropy_src, config));
}

static void setup_csrng(void) {
  dif_csrng_t csrng;
  CHECK_DIF_OK(dif_csrng_init(
      mmio_region_from_addr(TOP_EARLGREY_CSRNG_BASE_ADDR), &csrng));
  CHECK_DIF_OK(dif_csrng_configure(&csrng));
}

static void setup_edn(void) {
  // Temporary solution to configure/enable the EDN and CSRNG to allow OTBN to
  // run before a DIF is available,
  // https://github.com/lowRISC/opentitan/issues/6082
  mmio_region_write32(mmio_region_from_addr(TOP_EARLGREY_EDN0_BASE_ADDR),
                      EDN_CTRL_REG_OFFSET, 0x55aa);
  mmio_region_write32(mmio_region_from_addr(TOP_EARLGREY_EDN1_BASE_ADDR),
                      EDN_CTRL_REG_OFFSET, 0x55aa);
}

void entropy_testutils_boot_mode_init(void) {
  setup_entropy_src();
  setup_csrng();
  setup_edn();
}


/* sca_instruction has randomnization removed */

OTBN_DECLARE_APP_SYMBOLS(sca_instruction);
OTBN_DECLARE_SYMBOL_ADDR(sca_instruction, test_data);
OTBN_DECLARE_SYMBOL_ADDR(sca_instruction, dptr_test_data);


static const otbn_app_t kOtbnAppSCAInstruction = OTBN_APP_T_INIT(sca_instruction);
static const otbn_addr_t kOtbnDptrTestData = OTBN_ADDR_T_INIT(sca_instruction, dptr_test_data);
static const otbn_addr_t kOtbnTestData = OTBN_ADDR_T_INIT(sca_instruction, test_data);

/**
 * Makes a single dptr in the P256 library point to where its value is stored.
 */
static void setup_data_pointer(otbn_t *otbn_ctx, const otbn_addr_t dptr,
                               const otbn_addr_t value) {
  CHECK(otbn_copy_data_to_otbn(otbn_ctx, sizeof(value), &value, dptr) ==
        kOtbnOk);
}

/**
 * Sets up all data pointers used by the P256 library to point to DMEM.
 *
 * The ECDSA P256 OTBN library makes use of "named" data pointers as part of
 * its calling convention, which are exposed as symbol starting with `dptr_`.
 * The DMEM locations these pointers refer to is not mandated by the P256
 * calling convention; the values can be placed anywhere in OTBN DMEM.
 *
 * As convenience, `ecdsa_p256.s` pre-allocates space for the data values.
 *
 * This function makes the data pointers refer to the pre-allocated DMEM
 * regions to store the actual values.
 */
static void setup_data_pointers(otbn_t *otbn_ctx) {
  setup_data_pointer(otbn_ctx, kOtbnDptrTestData, kOtbnTestData);
}

static void simpleserial_cmd(const uint8_t *data, size_t data_len)
{
  otbn_t otbn_ctx;  
  dif_otbn_err_bits_t err_bits;
  uint32_t insn_cnt;
  CHECK(otbn_init(&otbn_ctx, mmio_region_from_addr(
                                 TOP_EARLGREY_OTBN_BASE_ADDR)) == kOtbnOk);
  CHECK(otbn_zero_data_memory(&otbn_ctx) == kOtbnOk);
  CHECK(otbn_load_app(&otbn_ctx, kOtbnAppSCAInstruction) == kOtbnOk);

  LOG_INFO("Loading Data");
  setup_data_pointers(&otbn_ctx);

  //TODO - think about how to load data. SimpleSerial has limited data size per command
  // (max 250 bytes). 
  CHECK(otbn_copy_data_to_otbn(&otbn_ctx, /*len_bytes=*/data_len, data,
                               kOtbnTestData) == kOtbnOk);

// Call OTBN to perform operation, and wait for it to complete.
  LOG_INFO("Running");
  sca_set_trigger_high();

  LOG_INFO("Execute");
  CHECK(otbn_execute(&otbn_ctx) == kOtbnOk);
  LOG_INFO("Wait for done");
  CHECK(otbn_busy_wait_for_done(&otbn_ctx) == kOtbnOk);
  sca_set_trigger_low();

  dif_otbn_get_insn_cnt(&otbn_ctx.dif, &insn_cnt);
  LOG_INFO("Debug: OTBN.INSN_CNT = 0x%x", insn_cnt);

  dif_otbn_get_err_bits(&otbn_ctx.dif, &err_bits);
  if (err_bits){
    LOG_INFO("OTBN Error Bits: %x", err_bits);
    if(err_bits & kDifOtbnErrBitsBadDataAddr){
      LOG_INFO(" -> kDifOtbnErrBitsBadDataAddr");
    }
  }
  LOG_INFO("Clearing OTBN memory");
  CHECK(otbn_zero_data_memory(&otbn_ctx) == kOtbnOk);

}

/**
 * Main function.
 *
 * Initializes peripherals and processes simple serial packets received over
 * UART.
 */
int main(void) {
  const dif_uart_t *uart1;

  entropy_testutils_boot_mode_init();

  sca_init(kScaTriggerSourceOtbn, kScaPeripheralOtbn | kScaPeripheralCsrng | kScaPeripheralEdn);
  sca_get_uart(&uart1);

  LOG_INFO("Running OTBN Instruction Counter serial");

  LOG_INFO("Initializing simple serial interface to capture board.");
  simple_serial_init(uart1);

  simple_serial_result_t err;
  if (err = simple_serial_register_handler('p', simpleserial_cmd), err != kSimpleSerialOk) {
      LOG_INFO("Register handler failed with return %hu", (short unsigned int)err);
      while(1);
  }

  sca_set_trigger_low(); //Manual trigger needed for now?

  LOG_INFO("Starting simple serial packet handling.");
  while (true) {
    simple_serial_process_packet();
  }
}
