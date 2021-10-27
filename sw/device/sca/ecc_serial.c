// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0


#include "sw/device/lib/dif/dif_otbn.h"
#include "sw/device/lib/runtime/ibex.h"
#include "sw/device/lib/runtime/log.h"
#include "sw/device/lib/runtime/otbn.h"
//#include "sw/device/lib/testing/check.h"
#include "sw/device/lib/testing/entropy_testutils.h"
#include "sw/device/lib/testing/test_framework/test_main.h"

#include "hw/top_earlgrey/sw/autogen/top_earlgrey.h"
#include "sw/device/sca/lib/prng.h"
#include "sw/device/sca/lib/sca.h"
#include "sw/device/sca/lib/simple_serial.h"


#define ASSERT(a) while(!(a))
// #define ASSERT(a) a


OTBN_DECLARE_APP_SYMBOLS(p256);       // The OTBN ECDSA/P-256 app.
OTBN_DECLARE_PTR_SYMBOL(p256, k);     // k
OTBN_DECLARE_PTR_SYMBOL(p256, rnd);     // random
OTBN_DECLARE_PTR_SYMBOL(p256, x);     // The public key x-coordinate.
OTBN_DECLARE_PTR_SYMBOL(p256, y);     // The public key y-coordinate.

/* Declare symbols for DMEM pointers */
OTBN_DECLARE_PTR_SYMBOL(p256, dptr_k);
OTBN_DECLARE_PTR_SYMBOL(p256, dptr_x);
OTBN_DECLARE_PTR_SYMBOL(p256, dptr_y);
OTBN_DECLARE_PTR_SYMBOL(p256, dptr_rnd);

static const otbn_app_t kOtbnAppP256ScalarMult = OTBN_APP_T_INIT(p256);

static const otbn_ptr_t kOtbnVarP256DptrK = OTBN_PTR_T_INIT(p256, dptr_k);
//static const otbn_ptr_t kOtbnVarP256DptrRnd =   OTBN_PTR_T_INIT(p256, dptr_rnd);
static const otbn_ptr_t kOtbnVarP256DptrX =   OTBN_PTR_T_INIT(p256, dptr_x);
static const otbn_ptr_t kOtbnVarP256DptrY =   OTBN_PTR_T_INIT(p256, dptr_y);



#define kP256ScalarNumWords 8

// TODO: This implementation waits while OTBN is processing; it should be
// modified to be non-blocking.
otbn_result_t ecdsa_p256_mult(const uint32_t x[],
                            const uint32_t y[],
                            const uint32_t k[],
                            uint32_t *result) {
  otbn_t otbn;
  int rtn;
  LOG_INFO("otbn_init");
  ASSERT(otbn_init(&otbn, mmio_region_from_addr(
                                 TOP_EARLGREY_OTBN_BASE_ADDR)) == kOtbnOk);
  LOG_INFO("otbn_zero");    
  rtn = otbn_zero_data_memory(&otbn);
  LOG_INFO("otbn_load");
  while(rtn == kOtbnOk);
  ASSERT(otbn_load_app(&otbn, kOtbnAppP256ScalarMult) == kOtbnOk);
  //RETURN_IF_ERROR(setup_data_pointers(&otbn));



  LOG_INFO("otbn copy");
  // Set the private key.*
  ASSERT(otbn_copy_data_to_otbn(&otbn, kP256ScalarNumWords,
                                         k, kOtbnVarP256DptrK) == kOtbnOk);
  ASSERT(otbn_copy_data_to_otbn(&otbn, kP256ScalarNumWords,
                                         x, kOtbnVarP256DptrX) == kOtbnOk);
  ASSERT(otbn_copy_data_to_otbn(&otbn, kP256ScalarNumWords,
                                         y, kOtbnVarP256DptrY) == kOtbnOk);
  // Start the OTBN routine. == kOtbnOk)
  LOG_INFO("otbn execute");
  ASSERT(otbn_execute(&otbn) == kOtbnOk);

  LOG_INFO("otbn wait");
  // Spin here waiting for OTBN to complete.
  ASSERT(otbn_busy_wait_for_done(&otbn) == kOtbnOk);

  LOG_INFO("done!");
  // Read signature R out of OTBN dmem.
  //RETURN_IF_ERROR(otbn_copy_data_from_otbn(&otbn, kP256ScalarNumWords,
  //                                         kOtbnVarEcdsaR, result->R));

  return kOtbnOk;
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

  sca_init(kScaTriggerSourceKmac, kScaPeripheralKmac);

  LOG_INFO("Running ecc_serial");
  LOG_INFO("Initializing simple serial interface to capture board.");
  sca_get_uart(&uart1);
  simple_serial_init(uart1);
  //simple_serial_register_handler('k', sha3_serial_set_key);
  //simple_serial_register_handler('p', sha3_serial_single_absorb);

  // Message
  uint8_t kIn[32] = {"Hello OTBN."};

  // Public key x-coordinate (Q.x)
  uint8_t kPublicKeyQx[32] = {
      0x4e, 0xb2, 0x8b, 0x55, 0xeb, 0x88, 0x62, 0x24, 0xf2, 0xbf, 0x1b,
      0x9e, 0xd8, 0x4a, 0x09, 0xa7, 0x86, 0x67, 0x92, 0xcd, 0xca, 0x07,
      0x5d, 0x07, 0x82, 0xe7, 0x2d, 0xac, 0x31, 0x14, 0x79, 0x1f};

  // Public key y-coordinate (Q.y)
  uint8_t kPublicKeyQy[32] = {
      0x27, 0x9c, 0xe4, 0x23, 0x24, 0x10, 0xa2, 0xfa, 0xbd, 0x53, 0x73,
      0xf1, 0xa5, 0x08, 0xf0, 0x40, 0x9e, 0xc0, 0x55, 0x21, 0xa4, 0xf0,
      0x54, 0x59, 0x00, 0x3e, 0x5f, 0x15, 0x3c, 0xc6, 0x4b, 0x87};

  uint8_t out[32];


  LOG_INFO("Running p256 test");
  ecdsa_p256_mult((uint32_t *)kPublicKeyQx, (uint32_t *)kPublicKeyQy, (uint32_t *)kIn, (uint32_t *)out);

  LOG_INFO("Starting simple serial packet handling.");
  while (true) {
    simple_serial_process_packet();
  }
}
