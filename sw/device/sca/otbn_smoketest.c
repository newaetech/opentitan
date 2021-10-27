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

OTBN_DECLARE_APP_SYMBOLS(barrett384);

static const otbn_app_t kAppBarrett = OTBN_APP_T_INIT(barrett384);

OTBN_DECLARE_APP_SYMBOLS(err_test);

static const otbn_app_t kAppErrTest = OTBN_APP_T_INIT(err_test);

const test_config_t kTestConfig;

#define kP256ScalarNumWords 8

#define ASSERT(a) while(!(a))

OTBN_DECLARE_APP_SYMBOLS(p256);       // The OTBN ECDSA/P-256 app.
OTBN_DECLARE_PTR_SYMBOL(p256, k);     // k
OTBN_DECLARE_PTR_SYMBOL(p256, rnd);     // random
OTBN_DECLARE_PTR_SYMBOL(p256, x);     // The public key x-coordinate.
OTBN_DECLARE_PTR_SYMBOL(p256, y);     // The public key y-coordinate.
OTBN_DECLARE_PTR_SYMBOL(p256, d);     // The private key

/* Declare symbols for DMEM pointers */
OTBN_DECLARE_PTR_SYMBOL(p256, dptr_k);
OTBN_DECLARE_PTR_SYMBOL(p256, dptr_x);
OTBN_DECLARE_PTR_SYMBOL(p256, dptr_y);
OTBN_DECLARE_PTR_SYMBOL(p256, dptr_d);
OTBN_DECLARE_PTR_SYMBOL(p256, dptr_rnd);

static const otbn_app_t kOtbnAppP256ScalarMult = OTBN_APP_T_INIT(p256);

//static const otbn_ptr_t kOtbnVarP256Rnd = OTBN_PTR_T_INIT(p256, rnd);
//static const otbn_ptr_t kOtbnVarP256K =   OTBN_PTR_T_INIT(p256, k);
//static const otbn_ptr_t kOtbnVarP256X =   OTBN_PTR_T_INIT(p256, x);
//static const otbn_ptr_t kOtbnVarP256Y =   OTBN_PTR_T_INIT(p256, y);

static const otbn_ptr_t kOtbnVarP256DptrK = OTBN_PTR_T_INIT(p256, dptr_k);
//static const otbn_ptr_t kOtbnVarP256DptrRnd =   OTBN_PTR_T_INIT(p256, dptr_rnd);
static const otbn_ptr_t kOtbnVarP256DptrX =   OTBN_PTR_T_INIT(p256, dptr_x);
static const otbn_ptr_t kOtbnVarP256DptrY =   OTBN_PTR_T_INIT(p256, dptr_y);
static const otbn_ptr_t kOtbnVarP256DptrD =   OTBN_PTR_T_INIT(p256, dptr_d);


// TODO: This implementation waits while OTBN is processing; it should be
// modified to be non-blocking.
otbn_result_t ecdsa_p256_mult(const uint32_t x[],
                            const uint32_t y[],
                            const uint32_t k[],
                            const uint32_t d[],
                            uint32_t *result) {
  otbn_t otbn;
  LOG_INFO("otbn_init");
  ASSERT(otbn_init(&otbn, mmio_region_from_addr(
                                 TOP_EARLGREY_OTBN_BASE_ADDR)) == kOtbnOk);
                                
  //uint32_t rnd[kP256ScalarNumWords] = {};
  size_t dmem_size_words =
      dif_otbn_get_dmem_size_bytes(&otbn.dif) / sizeof(uint32_t);
  LOG_INFO("%d", dmem_size_words);
  const uint32_t zero = 0;
  dif_otbn_dmem_write(&otbn.dif, sizeof(uint32_t), &zero,
                            sizeof(zero));
  // Load the ECDSA/P-256 app and set up data pointers
  LOG_INFO("otbn_zero");
  ASSERT(otbn_zero_data_memory(&otbn) == kOtbnOk);

  LOG_INFO("otbn_load");
  ASSERT(otbn_load_app(&otbn, kOtbnAppP256ScalarMult) == kOtbnOk);

  LOG_INFO("otbn copy");
  // Set the private key.*
  ASSERT(otbn_copy_data_to_otbn(&otbn, kP256ScalarNumWords,
                                         k, kOtbnVarP256DptrK) == kOtbnOk);
  ASSERT(otbn_copy_data_to_otbn(&otbn, kP256ScalarNumWords,
                                         x, kOtbnVarP256DptrX) == kOtbnOk);
  ASSERT(otbn_copy_data_to_otbn(&otbn, kP256ScalarNumWords,
                                         y, kOtbnVarP256DptrY) == kOtbnOk);
  ASSERT(otbn_copy_data_to_otbn(&otbn, kP256ScalarNumWords,
                                         d, kOtbnVarP256DptrD) == kOtbnOk);
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
 * Get OTBN error bits, check this succeeds and code matches `expected_err_bits`
 */
static void check_otbn_err_bits(otbn_t *otbn_ctx,
                                dif_otbn_err_bits_t expected_err_bits) {
  dif_otbn_err_bits_t otbn_err_bits;
  CHECK_DIF_OK(dif_otbn_get_err_bits(&otbn_ctx->dif, &otbn_err_bits));
  CHECK(otbn_err_bits == expected_err_bits,
        "dif_otbn_get_err_bits() produced unexpected error bits: %x",
        otbn_err_bits);
}

/**
 * Gets the OTBN instruction count, checks that it matches expectations.
 */
static void check_otbn_insn_cnt(otbn_t *otbn_ctx, uint32_t expected_insn_cnt) {
  uint32_t insn_cnt;
  CHECK_DIF_OK(dif_otbn_get_insn_cnt(&otbn_ctx->dif, &insn_cnt));
  CHECK(insn_cnt == expected_insn_cnt,
        "Expected to execute %d instructions, but got %d.", expected_insn_cnt,
        insn_cnt);
}

/**
 * Run a 384-bit Barrett Multiplication on OTBN and check its result.
 *
 * This test is not aiming to exhaustively test the Barrett multiplication
 * itself, but test the interaction between device software and OTBN. As such,
 * only trivial parameters are used.
 *
 * The code executed on OTBN can be found in sw/otbn/code-snippets/barrett384.s.
 * The entry point wrap_barrett384() is called according to the calling
 * convention described in the OTBN assembly code file.
 */
static void test_barrett384(otbn_t *otbn_ctx) {
  enum { kDataSizeBytes = 48 };

  CHECK(otbn_zero_data_memory(otbn_ctx) == kOtbnOk);
  CHECK(otbn_load_app(otbn_ctx, kAppBarrett) == kOtbnOk);

  // a, first operand
  static const uint8_t a[kDataSizeBytes] = {10};

  // b, second operand
  static uint8_t b[kDataSizeBytes] = {20};

  // m, modulus, max. length 384 bit with 2^384 > m > 2^383
  // We choose the modulus of P-384: m = 2**384 - 2**128 - 2**96 + 2**32 - 1
  static const uint8_t m[kDataSizeBytes] = {
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xff, 0xff,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff};

  // u, pre-computed Barrett constant (without u[384]/MSb of u which is always 1
  // for the allowed range but has to be set to 0 here).
  // u has to be pre-calculated as u = floor(2^768/m).
  static const uint8_t u[kDataSizeBytes] = {
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x01};

  // c, result, max. length 384 bit.
  uint8_t c[kDataSizeBytes] = {0};
  

  // c = (a * b) % m = (10 * 20) % m = 200
  static const uint8_t c_expected[kDataSizeBytes] = {200};

  // TODO: Use symbols from the application to load these parameters once they
  // are available (#3998).
  CHECK_DIF_OK(
      dif_otbn_dmem_write(&otbn_ctx->dif, /*offset_bytes=*/0, &a, sizeof(a)));
  CHECK_DIF_OK(
      dif_otbn_dmem_write(&otbn_ctx->dif, /*offset_bytes=*/64, &b, sizeof(b)));
  CHECK_DIF_OK(
      dif_otbn_dmem_write(&otbn_ctx->dif, /*offset_bytes=*/256, &m, sizeof(m)));
  CHECK_DIF_OK(
      dif_otbn_dmem_write(&otbn_ctx->dif, /*offset_bytes=*/320, &u, sizeof(u)));

  CHECK(dif_otbn_set_ctrl_software_errs_fatal(&otbn_ctx->dif, true) == kDifOk);
  CHECK(otbn_execute(otbn_ctx) == kOtbnOk);
  CHECK(dif_otbn_set_ctrl_software_errs_fatal(&otbn_ctx->dif, false) ==
        kDifUnavailable);
  CHECK(otbn_busy_wait_for_done(otbn_ctx) == kOtbnOk);

  // Reading back result (c).
  dif_otbn_dmem_read(&otbn_ctx->dif, 512, &c, sizeof(c));

  for (int i = 0; i < sizeof(c); ++i) {
    CHECK(c[i] == c_expected[i],
          "Unexpected result c at byte %d: 0x%x (actual) != 0x%x (expected)", i,
          c[i], c_expected[i]);
  }

  check_otbn_insn_cnt(otbn_ctx, 161);
}

/**
 * Run err_test on OTBN and check it produces the expected error
 *
 * This test tries to load from an invalid address which should result in the
 * kDifOtbnErrBitsBadDataAddr error bit being set
 *
 * The code executed on OTBN can be found in sw/otbn/code-snippets/err_test.s.
 * The entry point wrap_err_test() is called, no arguments are passed or results
 * returned.
 */
static void test_err_test(otbn_t *otbn_ctx) {
  CHECK(otbn_load_app(otbn_ctx, kAppErrTest) == kOtbnOk);

  // TODO: Turn on software_errs_fatal for err_test. Currently the model doesn't
  // support this feature so turning it on leads to a failure when run with the
  // model.
  CHECK(dif_otbn_set_ctrl_software_errs_fatal(&otbn_ctx->dif, false) == kDifOk);
  CHECK(otbn_execute(otbn_ctx) == kOtbnOk);
  CHECK(otbn_busy_wait_for_done(otbn_ctx) == kOtbnOperationFailed);

  check_otbn_err_bits(otbn_ctx, kDifOtbnErrBitsBadDataAddr);

  check_otbn_insn_cnt(otbn_ctx, 1);
}

bool test_main() {
  entropy_testutils_boot_mode_init();

  otbn_t otbn_ctx;
  CHECK(otbn_init(&otbn_ctx, mmio_region_from_addr(
                                 TOP_EARLGREY_OTBN_BASE_ADDR)) == kOtbnOk);

  test_barrett384(&otbn_ctx);
  test_err_test(&otbn_ctx);

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


  // Private key (d)
  static const uint8_t kPrivateKeyD[32] = {
      0xcd, 0xb4, 0x57, 0xaf, 0x1c, 0x9f, 0x4c, 0x74, 0x02, 0x0c, 0x7e,
      0x8b, 0xe9, 0x93, 0x3e, 0x28, 0x0c, 0xf0, 0x18, 0x0d, 0xf4, 0x6c,
      0x0b, 0xda, 0x7a, 0xbb, 0xe6, 0x8f, 0xb7, 0xa0, 0x45, 0x55};

  uint8_t out[32];


  LOG_INFO("Running p256 test");
  ecdsa_p256_mult((uint32_t *)kPublicKeyQx, (uint32_t *)kPublicKeyQy, (uint32_t *)kIn, (uint32_t *)kPrivateKeyD, (uint32_t *)out);

  return true;
}
