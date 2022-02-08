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


/* p384_ecdsa_sca has randomnization removed */

OTBN_DECLARE_APP_SYMBOLS(p384_ecdsa_sca);

OTBN_DECLARE_SYMBOL_ADDR(p384_ecdsa_sca, dptr_msg);
OTBN_DECLARE_SYMBOL_ADDR(p384_ecdsa_sca, dptr_r);
OTBN_DECLARE_SYMBOL_ADDR(p384_ecdsa_sca, dptr_s);
OTBN_DECLARE_SYMBOL_ADDR(p384_ecdsa_sca, dptr_x);
OTBN_DECLARE_SYMBOL_ADDR(p384_ecdsa_sca, dptr_y);
OTBN_DECLARE_SYMBOL_ADDR(p384_ecdsa_sca, dptr_d);
OTBN_DECLARE_SYMBOL_ADDR(p384_ecdsa_sca, dptr_rnd); //x_r not used in p384 verify .s
OTBN_DECLARE_SYMBOL_ADDR(p384_ecdsa_sca, dptr_k);

OTBN_DECLARE_SYMBOL_ADDR(p384_ecdsa_sca, mode);
OTBN_DECLARE_SYMBOL_ADDR(p384_ecdsa_sca, msg);
OTBN_DECLARE_SYMBOL_ADDR(p384_ecdsa_sca, r);
OTBN_DECLARE_SYMBOL_ADDR(p384_ecdsa_sca, s);
OTBN_DECLARE_SYMBOL_ADDR(p384_ecdsa_sca, x);
OTBN_DECLARE_SYMBOL_ADDR(p384_ecdsa_sca, y);
OTBN_DECLARE_SYMBOL_ADDR(p384_ecdsa_sca, d);
OTBN_DECLARE_SYMBOL_ADDR(p384_ecdsa_sca, k);
OTBN_DECLARE_SYMBOL_ADDR(p384_ecdsa_sca, rnd); //x_r not used in p384 verify .s file


static const otbn_app_t kOtbnAppP384Ecdsa = OTBN_APP_T_INIT(p384_ecdsa_sca);

static const otbn_addr_t kOtbnVarDptrMsg = OTBN_ADDR_T_INIT(p384_ecdsa_sca, dptr_msg);
static const otbn_addr_t kOtbnVarDptrR = OTBN_ADDR_T_INIT(p384_ecdsa_sca, dptr_r);
static const otbn_addr_t kOtbnVarDptrS = OTBN_ADDR_T_INIT(p384_ecdsa_sca, dptr_s);
static const otbn_addr_t kOtbnVarDptrX = OTBN_ADDR_T_INIT(p384_ecdsa_sca, dptr_x);
static const otbn_addr_t kOtbnVarDptrY = OTBN_ADDR_T_INIT(p384_ecdsa_sca, dptr_y);
static const otbn_addr_t kOtbnVarDptrD = OTBN_ADDR_T_INIT(p384_ecdsa_sca, dptr_d);
static const otbn_addr_t kOtbnVarDptrRnd = OTBN_ADDR_T_INIT(p384_ecdsa_sca, dptr_rnd);
static const otbn_addr_t kOtbnVarDptrK = OTBN_ADDR_T_INIT(p384_ecdsa_sca, dptr_k);

static const otbn_addr_t kOtbnVarMode = OTBN_ADDR_T_INIT(p384_ecdsa_sca, mode);
static const otbn_addr_t kOtbnVarMsg = OTBN_ADDR_T_INIT(p384_ecdsa_sca, msg);
static const otbn_addr_t kOtbnVarR = OTBN_ADDR_T_INIT(p384_ecdsa_sca, r);
static const otbn_addr_t kOtbnVarS = OTBN_ADDR_T_INIT(p384_ecdsa_sca, s);
static const otbn_addr_t kOtbnVarX = OTBN_ADDR_T_INIT(p384_ecdsa_sca, x);
static const otbn_addr_t kOtbnVarY = OTBN_ADDR_T_INIT(p384_ecdsa_sca, y);
static const otbn_addr_t kOtbnVarD = OTBN_ADDR_T_INIT(p384_ecdsa_sca, d);
static const otbn_addr_t kOtbnVarRnd = OTBN_ADDR_T_INIT(p384_ecdsa_sca, rnd);
static const otbn_addr_t kOtbnVarK = OTBN_ADDR_T_INIT(p384_ecdsa_sca, k);

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
  setup_data_pointer(otbn_ctx, kOtbnVarDptrMsg, kOtbnVarMsg);
  setup_data_pointer(otbn_ctx, kOtbnVarDptrR, kOtbnVarR);
  setup_data_pointer(otbn_ctx, kOtbnVarDptrS, kOtbnVarS);
  setup_data_pointer(otbn_ctx, kOtbnVarDptrX, kOtbnVarX);
  setup_data_pointer(otbn_ctx, kOtbnVarDptrY, kOtbnVarY);
  setup_data_pointer(otbn_ctx, kOtbnVarDptrD, kOtbnVarD);
  setup_data_pointer(otbn_ctx, kOtbnVarDptrRnd, kOtbnVarRnd);
  setup_data_pointer(otbn_ctx, kOtbnVarDptrK, kOtbnVarK);
}


/**
 * Signs a message with ECDSA using the P-384 curve.
 *
 * @param otbn_ctx            The OTBN context object.
 * @param msg                 The message to sign (32B).
 * @param private_key_d       The private key (32B).
 * @param[out] signature_r    Signature component r (the x-coordinate of R).
 *                            Provide a pre-allocated 32B buffer.
 * @param[out] signature_s    Signature component s (the proof).
 *                            Provide a pre-allocated 32B buffer.
 */
static void p384_ecdsa_sign(otbn_t *otbn_ctx, const uint8_t *msg,
                            const uint8_t *private_key_d, uint8_t *signature_r,
                            uint8_t *signature_s, const uint8_t *k) {
  CHECK(otbn_ctx != NULL);

  // Set pointers to input arguments.
  LOG_INFO("Setup data pointers");
  setup_data_pointers(otbn_ctx);

  // char random_k[32] = {0x14, 0};

  // Write input arguments.
  uint32_t mode = 1;  // mode 1 => sign
  LOG_INFO("Copy data");
  CHECK(otbn_copy_data_to_otbn(otbn_ctx, sizeof(mode), &mode, kOtbnVarMode) ==
        kOtbnOk);
  CHECK(otbn_copy_data_to_otbn(otbn_ctx, /*len_bytes=*/48, msg, kOtbnVarMsg) ==
        kOtbnOk);
  CHECK(otbn_copy_data_to_otbn(otbn_ctx, /*len_bytes=*/48, private_key_d,
                               kOtbnVarD) == kOtbnOk);

  CHECK(otbn_copy_data_to_otbn(otbn_ctx, /*len_bytes=*/48, k,
                               kOtbnVarK) == kOtbnOk);

  // Call OTBN to perform operation, and wait for it to complete.
  LOG_INFO("Execute");
  CHECK(otbn_execute(otbn_ctx) == kOtbnOk);
  LOG_INFO("Wait for done");
  CHECK(otbn_busy_wait_for_done(otbn_ctx) == kOtbnOk);

  // Read back results.
  LOG_INFO("Get results");
  CHECK(otbn_copy_data_from_otbn(otbn_ctx, /*len_bytes=*/48, kOtbnVarR,
                                 signature_r) == kOtbnOk);
  CHECK(otbn_copy_data_from_otbn(otbn_ctx, /*len_bytes=*/48, kOtbnVarS,
                                 signature_s) == kOtbnOk);
  LOG_INFO("r[0]: 0x%02x", signature_r[0]);
  LOG_INFO("s[0]: 0x%02x", signature_s[0]);
}

static void simpleserial_ecdsa(const uint8_t *data, size_t data_len)
{
  otbn_t otbn_ctx;
  static const uint8_t kPrivateKeyD[32] = {
      0xcd, 0xb4, 0x57, 0xaf, 0x1c, 0x9f, 0x4c, 0x74, 0x02, 0x0c, 0x7e,
      0x8b, 0xe9, 0x93, 0x3e, 0x28, 0x0c, 0xf0, 0x18, 0x0d, 0xf4, 0x6c,
      0x0b, 0xda, 0x7a, 0xbb, 0xe6, 0x8f, 0xb7, 0xa0, 0x45, 0x55};
  static const uint8_t kIn[48] = {"Hello OTBN."};
    if (data_len != 48) {
      LOG_INFO("Invalid data length %hu", (uint8_t )data_len);
      return;
    }
  LOG_INFO("SSECDSA starting...");
  CHECK(otbn_init(&otbn_ctx, mmio_region_from_addr(
                                 TOP_EARLGREY_OTBN_BASE_ADDR)) == kOtbnOk);
  CHECK(otbn_zero_data_memory(&otbn_ctx) == kOtbnOk);
  CHECK(otbn_load_app(&otbn_ctx, kOtbnAppP384Ecdsa) == kOtbnOk);

  // Sign
  uint8_t signature_r[48] = {0};
  uint8_t signature_s[48] = {0};

  LOG_INFO("Signing");
  sca_set_trigger_high();
  p384_ecdsa_sign(&otbn_ctx, kIn, kPrivateKeyD, signature_r, signature_s, data);
  sca_set_trigger_low();
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

  LOG_INFO("Running ECC serial");

  LOG_INFO("Initializing simple serial interface to capture board.");
  simple_serial_init(uart1);

  simple_serial_result_t err;
  if (err = simple_serial_register_handler('p', simpleserial_ecdsa), err != kSimpleSerialOk) {
      LOG_INFO("Register handler failed with return %hu", (short unsigned int)err);
      while(1);
  }

  sca_set_trigger_low(); //Manual trigger needed for now?

  LOG_INFO("Starting simple serial packet handling.");
  while (true) {
    simple_serial_process_packet();
  }
}
