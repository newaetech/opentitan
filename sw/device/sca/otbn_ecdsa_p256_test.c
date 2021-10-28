// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#include "sw/device/lib/dif/dif_otbn.h"
#include "sw/device/lib/runtime/ibex.h"
#include "sw/device/lib/runtime/log.h"
#include "sw/device/lib/runtime/otbn.h"
#include "sw/device/lib/testing/check.h"
#include "sw/device/lib/testing/entropy_testutils.h"
#include "sw/device/lib/testing/test_framework/test_main.h"

#include "hw/top_earlgrey/sw/autogen/top_earlgrey.h"
#include "sw/device/sca/lib/sca.h"
#include "sw/device/sca/lib/simple_serial.h"
#include "sw/device/sca/lib/prng.h"

#define IGNORE_RESULT(expr) \
  if (expr) {               \
  }
/**
 * ECDSA sign and verify test with the NIST P-256 curve using OTBN.
 *
 * IMPORTANT: This test is not a secure, complete, or reusable implementation of
 * a cryptographic algorithm; it is not even close to being production-ready.
 * It is only meant as an end-to-end test for OTBN during the bringup phase.
 *
 * The test contains constants and expected output, which can be independently
 * and conveniently verified using a Python script.
 *
 * <code>
 * # Optional: generate a new key
 * $ openssl ecparam -name prime256v1 -genkey -noout -out \
 *     otbn_ecdsa_p256_test_private_key.pem
 *
 * # Create all constants/variables
 * $ ./otbn_test_params.py ecc otbn_ecdsa_p256_test_private_key.pem
 * </code>
 */

OTBN_DECLARE_APP_SYMBOLS(p256_ecdsa);

OTBN_DECLARE_PTR_SYMBOL(p256_ecdsa, dptr_msg);
OTBN_DECLARE_PTR_SYMBOL(p256_ecdsa, dptr_r);
OTBN_DECLARE_PTR_SYMBOL(p256_ecdsa, dptr_s);
OTBN_DECLARE_PTR_SYMBOL(p256_ecdsa, dptr_x);
OTBN_DECLARE_PTR_SYMBOL(p256_ecdsa, dptr_y);
OTBN_DECLARE_PTR_SYMBOL(p256_ecdsa, dptr_d);
OTBN_DECLARE_PTR_SYMBOL(p256_ecdsa, dptr_x_r);
OTBN_DECLARE_PTR_SYMBOL(p256_ecdsa, dptr_k);

OTBN_DECLARE_PTR_SYMBOL(p256_ecdsa, mode);
OTBN_DECLARE_PTR_SYMBOL(p256_ecdsa, msg);
OTBN_DECLARE_PTR_SYMBOL(p256_ecdsa, r);
OTBN_DECLARE_PTR_SYMBOL(p256_ecdsa, s);
OTBN_DECLARE_PTR_SYMBOL(p256_ecdsa, x);
OTBN_DECLARE_PTR_SYMBOL(p256_ecdsa, y);
OTBN_DECLARE_PTR_SYMBOL(p256_ecdsa, d);
OTBN_DECLARE_PTR_SYMBOL(p256_ecdsa, k);
OTBN_DECLARE_PTR_SYMBOL(p256_ecdsa, x_r);

static const otbn_app_t kOtbnAppP256Ecdsa = OTBN_APP_T_INIT(p256_ecdsa);

static const otbn_ptr_t kOtbnVarDptrMsg = OTBN_PTR_T_INIT(p256_ecdsa, dptr_msg);
static const otbn_ptr_t kOtbnVarDptrR = OTBN_PTR_T_INIT(p256_ecdsa, dptr_r);
static const otbn_ptr_t kOtbnVarDptrS = OTBN_PTR_T_INIT(p256_ecdsa, dptr_s);
static const otbn_ptr_t kOtbnVarDptrX = OTBN_PTR_T_INIT(p256_ecdsa, dptr_x);
static const otbn_ptr_t kOtbnVarDptrY = OTBN_PTR_T_INIT(p256_ecdsa, dptr_y);
static const otbn_ptr_t kOtbnVarDptrD = OTBN_PTR_T_INIT(p256_ecdsa, dptr_d);
static const otbn_ptr_t kOtbnVarDptrXR = OTBN_PTR_T_INIT(p256_ecdsa, dptr_x_r);
static const otbn_ptr_t kOtbnVarDptrK = OTBN_PTR_T_INIT(p256_ecdsa, dptr_k);

static const otbn_ptr_t kOtbnVarMode = OTBN_PTR_T_INIT(p256_ecdsa, mode);
static const otbn_ptr_t kOtbnVarMsg = OTBN_PTR_T_INIT(p256_ecdsa, msg);
static const otbn_ptr_t kOtbnVarR = OTBN_PTR_T_INIT(p256_ecdsa, r);
static const otbn_ptr_t kOtbnVarS = OTBN_PTR_T_INIT(p256_ecdsa, s);
static const otbn_ptr_t kOtbnVarX = OTBN_PTR_T_INIT(p256_ecdsa, x);
static const otbn_ptr_t kOtbnVarY = OTBN_PTR_T_INIT(p256_ecdsa, y);
static const otbn_ptr_t kOtbnVarD = OTBN_PTR_T_INIT(p256_ecdsa, d);
static const otbn_ptr_t kOtbnVarXR = OTBN_PTR_T_INIT(p256_ecdsa, x_r);
static const otbn_ptr_t kOtbnVarK = OTBN_PTR_T_INIT(p256_ecdsa, k);


const test_config_t kTestConfig;

/**
 * CHECK()s that the actual data matches the expected data.
 *
 * @param actual The actual data.
 * @param expected The expected data.
 * @param size_bytes The size of the actual/expected data.
 */
static void check_data(const char *msg, const uint8_t *actual,
                       const uint8_t *expected, size_t size_bytes) {
  for (int i = 0; i < size_bytes; ++i) {
    CHECK(actual[i] == expected[i],
          "%s: mismatch at byte %d: 0x%x (actual) != 0x%x (expected)", msg, i,
          actual[i], expected[i]);
  }
}

/**
 * Starts a profiling section.
 *
 * Call this function at the start of a section that should be profiled, and
 * call `profile_end()` at the end of it to display the results.
 *
 * @return The cycle counter when starting the profiling.
 */
static uint64_t profile_start(void) { return ibex_mcycle_read(); }

/**
 * Ends a profiling section.
 *
 * The time since `profile_start()` is printed as log message.
 *
 * @param t_start Start timestamp, as returned from profile_start().
 * @param msg Name of the operation (for logging purposes).
 */
static void profile_end(uint64_t t_start, const char *msg) {
  uint64_t t_end = ibex_mcycle_read();
  uint32_t cycles = t_end - t_start;
  uint32_t time_us = cycles / 100;
  LOG_INFO("%s took %u cycles or %u us @ 100 MHz.", msg, cycles, time_us);
}

/**
 * Makes a single dptr in the P256 library point to where its value is stored.
 */
static void setup_data_pointer(otbn_t *otbn_ctx, const otbn_ptr_t dptr,
                               const otbn_ptr_t value) {
  uint32_t value_dmem_addr;
  CHECK(otbn_data_ptr_to_dmem_addr(otbn_ctx, value, &value_dmem_addr) ==
        kOtbnOk);
  CHECK(otbn_copy_data_to_otbn(otbn_ctx, sizeof(value_dmem_addr),
                               &value_dmem_addr, dptr) == kOtbnOk);
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
  setup_data_pointer(otbn_ctx, kOtbnVarDptrXR, kOtbnVarXR);
  setup_data_pointer(otbn_ctx, kOtbnVarDptrK, kOtbnVarK);
}

/**
 * Signs a message with ECDSA using the P-256 curve.
 *
 * @param otbn_ctx            The OTBN context object.
 * @param msg                 The message to sign (32B).
 * @param private_key_d       The private key (32B).
 * @param[out] signature_r    Signature component r (the x-coordinate of R).
 *                            Provide a pre-allocated 32B buffer.
 * @param[out] signature_s    Signature component s (the proof).
 *                            Provide a pre-allocated 32B buffer.
 */
static void p256_ecdsa_sign(otbn_t *otbn_ctx, const uint8_t *msg,
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
  CHECK(otbn_copy_data_to_otbn(otbn_ctx, /*len_bytes=*/32, msg, kOtbnVarMsg) ==
        kOtbnOk);
  CHECK(otbn_copy_data_to_otbn(otbn_ctx, /*len_bytes=*/32, private_key_d,
                               kOtbnVarD) == kOtbnOk);

  CHECK(otbn_copy_data_to_otbn(otbn_ctx, /*len_bytes=*/32, k,
                               kOtbnVarK) == kOtbnOk);

  // Call OTBN to perform operation, and wait for it to complete.
  LOG_INFO("Execute");
  CHECK(otbn_execute(otbn_ctx) == kOtbnOk);
  LOG_INFO("Wait for done");
  CHECK(otbn_busy_wait_for_done(otbn_ctx) == kOtbnOk);

  // Read back results.
  LOG_INFO("Get results");
  CHECK(otbn_copy_data_from_otbn(otbn_ctx, /*len_bytes=*/32, kOtbnVarR,
                                 signature_r) == kOtbnOk);
  CHECK(otbn_copy_data_from_otbn(otbn_ctx, /*len_bytes=*/32, kOtbnVarS,
                                 signature_s) == kOtbnOk);
  LOG_INFO("%x", signature_r[0]);
  LOG_INFO("%x", signature_s[0]);
}

/**
 * Verifies a message with ECDSA using the P-256 curve.
 *
 * @param otbn_ctx             The OTBN context object.
 * @param msg                  The message to verify (32B).
 * @param signature_r          The signature component r (the proof) (32B).
 * @param signature_s          The signature component s (the proof) (32B).
 * @param public_key_x         The public key x-coordinate (32B).
 * @param public_key_y         The public key y-coordinate (32B).
 * @param[out] signature_x_r   Recovered point x_r (== R'.x). Provide a
 *                             pre-allocated 32B buffer.
 */
static void p256_ecdsa_verify(otbn_t *otbn_ctx, const uint8_t *msg,
                              const uint8_t *signature_r,
                              const uint8_t *signature_s,
                              const uint8_t *public_key_x,
                              const uint8_t *public_key_y,
                              uint8_t *signature_x_r) {
  CHECK(otbn_ctx != NULL);

  // Set pointers to input arguments.
  setup_data_pointers(otbn_ctx);

  // Write input arguments.
  uint32_t mode = 2;  // mode 2 => verify
  CHECK(otbn_copy_data_to_otbn(otbn_ctx, sizeof(mode), &mode, kOtbnVarMode) ==
        kOtbnOk);
  CHECK(otbn_copy_data_to_otbn(otbn_ctx, /*len_bytes=*/32, msg, kOtbnVarMsg) ==
        kOtbnOk);
  CHECK(otbn_copy_data_to_otbn(otbn_ctx, /*len_bytes=*/32, signature_r,
                               kOtbnVarR) == kOtbnOk);
  CHECK(otbn_copy_data_to_otbn(otbn_ctx, /*len_bytes=*/32, signature_s,
                               kOtbnVarS) == kOtbnOk);
  CHECK(otbn_copy_data_to_otbn(otbn_ctx, /*len_bytes=*/32, public_key_x,
                               kOtbnVarX) == kOtbnOk);
  CHECK(otbn_copy_data_to_otbn(otbn_ctx, /*len_bytes=*/32, public_key_y,
                               kOtbnVarY) == kOtbnOk);

  // Call OTBN to perform operation, and wait for it to complete.
  CHECK(otbn_execute(otbn_ctx) == kOtbnOk);
  CHECK(otbn_busy_wait_for_done(otbn_ctx) == kOtbnOk);

  // Read back results.
  CHECK(otbn_copy_data_from_otbn(otbn_ctx, /*len_bytes=*/32, kOtbnVarXR,
                                 signature_x_r) == kOtbnOk);
}

static void p256_ecdsa_point_mul(otbn_t *otbn_ctx,
                            const uint8_t *public_key_x,
                            const uint8_t *public_key_y) {
  CHECK(otbn_ctx != NULL);

  // Set pointers to input arguments.
  setup_data_pointers(otbn_ctx);

  // Write input arguments.
  uint32_t mode = 3;  // mode 1 => sign
  CHECK(otbn_copy_data_to_otbn(otbn_ctx, sizeof(mode), &mode, kOtbnVarMode) ==
        kOtbnOk);
  CHECK(otbn_copy_data_to_otbn(otbn_ctx, /*len_bytes=*/32, public_key_x,
                               kOtbnVarX) == kOtbnOk);
  CHECK(otbn_copy_data_to_otbn(otbn_ctx, /*len_bytes=*/32, public_key_y,
                               kOtbnVarY) == kOtbnOk);

  // Call OTBN to perform operation, and wait for it to complete.
  CHECK(otbn_execute(otbn_ctx) == kOtbnOk);
  otbn_busy_wait_for_done(otbn_ctx); // == kOtbnOk);

  dif_otbn_err_bits_t err_bits;
  dif_otbn_get_err_bits(&otbn_ctx->dif, &err_bits);
  LOG_INFO("ERROR BITS %x", err_bits);

  uint8_t output_x[32];
  uint8_t output_y[32];

// Read back results.
  CHECK(otbn_copy_data_from_otbn(otbn_ctx, /*len_bytes=*/32, kOtbnVarX,
                                 output_x) == kOtbnOk);
// Read back results.
  CHECK(otbn_copy_data_from_otbn(otbn_ctx, /*len_bytes=*/32, kOtbnVarY,
                                 output_y) == kOtbnOk);

  LOG_INFO("%x", output_x[0]);

}

/**
 * Performs a ECDSA roundtrip test using the NIST P-256 curve.
 *
 * A roundtrip consists of three steps: Initialize OTBN, sign, and verify.
 */
  otbn_t otbn_ctx;
  static const uint8_t kPrivateKeyD[32] = {
      0xcd, 0xb4, 0x57, 0xaf, 0x1c, 0x9f, 0x4c, 0x74, 0x02, 0x0c, 0x7e,
      0x8b, 0xe9, 0x93, 0x3e, 0x28, 0x0c, 0xf0, 0x18, 0x0d, 0xf4, 0x6c,
      0x0b, 0xda, 0x7a, 0xbb, 0xe6, 0x8f, 0xb7, 0xa0, 0x45, 0x55};
static void test_ecdsa_p256_roundtrip(void) {
  // Message
  static const uint8_t kIn[32] = {"Hello OTBN."};
/*
  // Public key x-coordinate (Q.x)
  static const uint8_t kPublicKeyQx[32] = {
      0x4e, 0xb2, 0x8b, 0x55, 0xeb, 0x88, 0x62, 0x24, 0xf2, 0xbf, 0x1b,
      0x9e, 0xd8, 0x4a, 0x09, 0xa7, 0x86, 0x67, 0x92, 0xcd, 0xca, 0x07,
      0x5d, 0x07, 0x82, 0xe7, 0x2d, 0xac, 0x31, 0x14, 0x79, 0x1f};

  // Public key y-coordinate (Q.y)
  static const uint8_t kPublicKeyQy[32] = {
      0x27, 0x9c, 0xe4, 0x23, 0x24, 0x10, 0xa2, 0xfa, 0xbd, 0x53, 0x73,
      0xf1, 0xa5, 0x08, 0xf0, 0x40, 0x9e, 0xc0, 0x55, 0x21, 0xa4, 0xf0,
      0x54, 0x59, 0x00, 0x3e, 0x5f, 0x15, 0x3c, 0xc6, 0x4b, 0x87};
*/
  // Private key (d)

  // Initialize
  uint64_t t_start_init = profile_start();
  CHECK(otbn_init(&otbn_ctx, mmio_region_from_addr(
                                 TOP_EARLGREY_OTBN_BASE_ADDR)) == kOtbnOk);
  CHECK(otbn_load_app(&otbn_ctx, kOtbnAppP256Ecdsa) == kOtbnOk);
  profile_end(t_start_init, "Initialization");

  // Sign
  uint8_t signature_r[32] = {0};
  uint8_t signature_s[32] = {0};

  LOG_INFO("Signing");
  uint64_t t_start_sign = profile_start();
  p256_ecdsa_sign(&otbn_ctx, kIn, kPrivateKeyD, signature_r, signature_s, kIn);
  profile_end(t_start_sign, "Sign");
  LOG_INFO("Clearing OTBN memory");
  CHECK(otbn_zero_data_memory(&otbn_ctx) == kOtbnOk);
}

static void simpleserial_ecdsa(const uint8_t *data, size_t data_len)
{
  static const uint8_t kIn[32] = {"Hello OTBN."};
    if (data_len != 32) {
      LOG_INFO("Invalid data length %hu", (uint8_t )data_len);
      return;
    }
  LOG_INFO("SSECDSA starting...");
  uint64_t t_start_init = profile_start();
  CHECK(otbn_init(&otbn_ctx, mmio_region_from_addr(
                                 TOP_EARLGREY_OTBN_BASE_ADDR)) == kOtbnOk);
  CHECK(otbn_zero_data_memory(&otbn_ctx) == kOtbnOk);
  CHECK(otbn_load_app(&otbn_ctx, kOtbnAppP256Ecdsa) == kOtbnOk);
  profile_end(t_start_init, "Initialization");

  // Sign
  uint8_t signature_r[32] = {0};
  uint8_t signature_s[32] = {0};

  LOG_INFO("Signing");
  sca_set_trigger_high();
  p256_ecdsa_sign(&otbn_ctx, kIn, kPrivateKeyD, signature_r, signature_s, data);
  sca_set_trigger_low();
  LOG_INFO("Clearing OTBN memory");
  CHECK(otbn_zero_data_memory(&otbn_ctx) == kOtbnOk);

}

const dif_uart_t *uart1;
static void try_serial(void) {
      simple_serial_result_t err;
      if (err = simple_serial_register_handler('p', simpleserial_ecdsa), err != kSimpleSerialOk) {
          LOG_INFO("Register handler failed with return %hu", (short unsigned int)err);
      }
  LOG_INFO("Starting simple serial packet handling.");
      // uint8_t x = 0;
  while (true) {
    simple_serial_process_packet();
  }
}

bool test_main() {
  entropy_testutils_boot_mode_init();
      sca_init(kScaTriggerSourceOtbn, kScaPeripheralOtbn | kScaPeripheralCsrng | kScaPeripheralEdn);
      sca_get_uart(&uart1);
      simple_serial_init(uart1);

  // test_ecdsa_p256_roundtrip();
  sca_set_trigger_low();

  try_serial();

  return true;
}
