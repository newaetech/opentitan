// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#ifndef OPENTITAN_SW_DEVICE_SCA_LIB_SIMPLE_SERIAL_H_
#define OPENTITAN_SW_DEVICE_SCA_LIB_SIMPLE_SERIAL_H_

#include <stddef.h>
#include <stdint.h>

#include "sw/device/lib/dif/dif_base.h"
#include "sw/device/lib/dif/dif_uart.h"

/**
 * @file
 * @brief Simple serial protocol for side-channel analysis.
 *
 * This library implements simple serial protocol version 1.1 and provides
 * built-in handlers for 'v' (version) and 's' (seed PRNG) commands. Clients
 * can implement additional command by registering their handlers using
 * `simple_serial_register_handler()`. See https://wiki.newae.com/SimpleSerial
 * for details on the protocol.
 */

/**
 * Sends an error message over UART if condition evaluates to false.
 */
#define SS_CHECK(condition)                          \
  do {                                               \
    if (!(condition)) {                              \
      simple_serial_send_status(kSimpleSerialError); \
      return;                                        \
    }                                                \
  } while (false)

/**
 * Sends an error message over UART if DIF does not return kDifOk.
 */
#define SS_CHECK_DIF_OK(dif_call)                    \
  do {                                               \
    if (dif_call != kDifOk) {                        \
      simple_serial_send_status(kSimpleSerialError); \
      return;                                        \
    }                                                \
  } while (false)

/**
 * Simple serial status codes.
 */
typedef enum simple_serial_result {
  kSimpleSerialOk = 0,
  kSimpleSerialError = 1,
} simple_serial_result_t;

/**
 * Command handlers must conform to this prototype.
 */
typedef void (*simple_serial_command_handler)(const uint8_t *, size_t);

/**
 * Initializes the data structures used by simple serial.
 *
 * This function also registers handlers for 'v' (version) and 's' (seed PRNG)
 * commands.
 *
 * @param uart Handle to an initialized UART device.
 */
void simple_serial_init(const dif_uart_t *uart);

/**
 * Registers a handler for a simple serial command.
 *
 * Clients cannot register handlers for 'v' (version) and 's' (seed PRNG)
 * commands since these are handled by this library.
 *
 * @param cmd Simple serial command.
 * @param handler Command handler.
 */
simple_serial_result_t simple_serial_register_handler(
    uint8_t cmd, simple_serial_command_handler handler);

/**
 * Waits for a simple serial packet and dispatches it to the appropriate
 * handler.
 */
void simple_serial_process_packet(void);

/**
 * Sends a simple serial packet over UART.
 *
 * @param cmd Simple serial command.
 * @param data Packet payload.
 * @param data_len Payload length.
 */
void simple_serial_send_packet(const uint8_t cmd, const uint8_t *data,
                               size_t data_len);

/**
 * Sends a simple serial status packer over UART.
 *
 * @param res Status code.
 */
void simple_serial_send_status(uint8_t res);

/**
 * Sends a buffer over UART as a hex encoded string.
 *
 * @param data A buffer
 * @param data_len Size of the buffer.
 */
void simple_serial_print_hex(const uint8_t *data, size_t data_len);

#ifndef SIMPLESERIAL_H
#define SIMPLESERIAL_H

#include <stdint.h>

#define SS_VER_1_0 0
#define SS_VER_1_1 1
#define SS_VER_2_0 2
#define SS_VER_2_1 3
#define SS_VER SS_VER_2_1

// Set up the SimpleSerial module
// This prepares any internal commands
void simpleserial_init(void);

// Add a command to the SimpleSerial module
// Args:
// - c:   The character designating this command
// - len: The number of bytes expected
// - fp:  A pointer to a callback, which is called after receiving data
// - fl:  Bitwise OR'd CMD_FLAG_* values. Defaults to CMD_FLAG_NONE when
//        calling simpleserial_addcmd()
// Example: simpleserial_addcmd('p', 16, encrypt_text)
// - Calls encrypt_text() with a 16 byte array after receiving a line
//   like p00112233445566778899AABBCCDDEEFF\n
// Notes:
// - Maximum of 10 active commands
// - Maximum length of 64 bytes
// - Returns 1 if either of these fail; otherwise 0
// - The callback function returns a number in [0x00, 0xFF] as a status code;
//   in protocol v1.1, this status code is returned through a "z" message
#if SS_VER == SS_VER_2_1
int simpleserial_addcmd(char c, unsigned int len, uint8_t (*fp)(uint8_t, uint8_t, uint8_t, uint8_t*));
#else

#define CMD_FLAG_NONE	0x00
// If this flag is set, the command supports variable length payload.
// The first byte (hex-encoded) indicates the length.
#define CMD_FLAG_LEN	0x01

int simpleserial_addcmd_flags(char c, unsigned int len, uint8_t (*fp)(uint8_t*, uint8_t), uint8_t);
int simpleserial_addcmd(char c, unsigned int len, uint8_t (*fp)(uint8_t*, uint8_t));
#endif

// Attempt to process a command
// If a full string is found, the relevant callback function is called
// Might return without calling a callback for several reasons:
// - First character didn't match any known commands
// - One of the characters wasn't in [0-9|A-F|a-f]
// - Data was too short or too long
void simpleserial_get(void);

// Write some data to the serial port
// Prepends the character c to the start of the line
// Example: simpleserial_put('r', 16, ciphertext)
void simpleserial_put(char c, uint8_t size, uint8_t* output);

typedef enum ss_err_cmd {
	SS_ERR_OK,
	SS_ERR_CMD,
	SS_ERR_CRC,
	SS_ERR_TIMEOUT,
    SS_ERR_LEN,
    SS_ERR_FRAME_BYTE
} ss_err_cmd;

#endif // SIMPLESERIAL_H
#endif  // OPENTITAN_SW_DEVICE_SCA_LIB_SIMPLE_SERIAL_H_
