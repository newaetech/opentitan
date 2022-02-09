/* Copyright lowRISC contributors. */
/* Licensed under the Apache License, Version 2.0, see LICENSE for details. */
/* SPDX-License-Identifier: Apache-2.0 */

/**
 * Standalone test for SCA measurement on instructions.
 */

.section .text.start
.global .start
sca_instructions:

  /* Set Up Pointers */
  la       x2, test_data
  la       x3, dptr_test_data
  sw       x2, 0(x3)

  li        x4, 0
    
  bn.lid    x4++, 0(x2)   /* w0 */
  bn.lid    x4++, 32(x2)  /* w1 */
  bn.lid    x4++, 64(x2)  /* w2 */
  bn.lid    x4++, 96(x2)  /* w3 */
  bn.lid    x4++, 128(x2) /* w4 */

  /* NOPs to allow stable powerlines */
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP

  /* Code Under Test */ 
  BN.XOR w0, w1,  w2
  BN.XOR w0, w1,  w2
  BN.XOR w0, w1,  w2
  BN.XOR w0, w1,  w2


  BN.XOR w3, w4,  w5
  BN.XOR w3, w4,  w5
  BN.XOR w3, w4,  w5
  BN.XOR w3, w4,  w5

  /* NOPs to end */
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP

  ecall


.data

/* Data */
.globl test_data
.balign 32
test_data:
  .zero 1312

/* pointer to Test Data (dptr_test_data) */
.globl dptr_test_data
.balign 4
dptr_test_data:
  .zero 4