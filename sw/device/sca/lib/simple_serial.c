// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#include "sw/device/sca/lib/simple_serial.h"

#include "sw/device/lib/arch/device.h"
#include "sw/device/lib/base/memory.h"
#include "sw/device/lib/dif/dif_uart.h"
#include "sw/device/lib/runtime/print.h"
#include "sw/device/sca/lib/prng.h"

#include "hw/top_earlgrey/sw/autogen/top_earlgrey.h"

/**
 * Macro for ignoring return values.
 *
 * This is needed because ‘(void)expr;` does not work for gcc.
 */
#define IGNORE_RESULT(expr) \
  if (expr) {               \
  }

// enum {
//   /**
//    * Simple serial protocol version 1.1.
//    */
//   kSimpleSerialProtocolVersion = 1,
//   kUartMaxRxPacketSize = 64,
// };

// /**
//  * Command handlers.
//  *
//  * Clients can register handlers for commands 'a'-'z' using
//  * `simple_serial_register_handler()` except for 'v' (version) and 's' (seed
//  * PRNG), which are handled by this library. This array has an extra element
//  * (27) that is initialized in `simple_serial_init()` to point to
//  * `simple_serial_unknown_command()` in order to simplify handling of invalid
//  * commands in `simple_serial_process_packet()`.
//  */
// static simple_serial_command_handler handlers[27];
// static const dif_uart_t *uart;

// static bool simple_serial_is_valid_command(uint8_t cmd) {
//   return cmd >= 'a' && cmd <= 'z';
// }

// /**
//  * Converts a hex encoded nibble to binary.
//  *
//  * @param hex A hex encoded nibble.
//  * @param[out] val Value of the nibble.
//  *
//  * @return Result of the operation.
//  */
// static simple_serial_result_t simple_serial_hex_to_bin(uint8_t hex,
//                                                        uint8_t *val) {
//   if (hex >= '0' && hex <= '9') {
//     *val = hex - '0';
//   } else if (hex >= 'A' && hex <= 'F') {
//     *val = hex - 'A' + 10;
//   } else if (hex >= 'a' && hex <= 'f') {
//     *val = hex - 'a' + 10;
//   } else {
//     return kSimpleSerialError;
//   }
//   return kSimpleSerialOk;
// }

// /**
//  * Receives a simple serial packet over UART.
//  *
//  * Simple serial packets are composed of:
//  * - Command: A single byte character,
//  * - Payload: A variable length hex encoded string,
//  * - Terminator: '\n'.
//  *
//  * @param[out] cmd Simple serial command.
//  * @param[out] data Buffer for received packet payload.
//  * @param data_buf_len Length of the packet payload buffer.
//  * @param[out] data_len Received packet payload length.
//  */
// static void simple_serial_receive_packet(uint8_t *cmd, uint8_t *data,
//                                          size_t data_buf_len,
//                                          size_t *data_len) {
//   while (true) {
//     // Read command byte - a single character.
//     IGNORE_RESULT(dif_uart_byte_receive_polled(uart, cmd));
//     if (*cmd == '\n') {
//       continue;
//     }
//     *data_len = 0;
//     // Read payload - a variable length hex encoded string terminated with '\n'.
//     do {
//       uint8_t hex_byte[2];
//       IGNORE_RESULT(dif_uart_byte_receive_polled(uart, &hex_byte[0]));
//       if (hex_byte[0] == '\n') {
//         return;
//       }
//       if (simple_serial_hex_to_bin(hex_byte[0], &hex_byte[0]) !=
//           kSimpleSerialOk) {
//         break;
//       }
//       IGNORE_RESULT(dif_uart_byte_receive_polled(uart, &hex_byte[1]));
//       if (simple_serial_hex_to_bin(hex_byte[1], &hex_byte[1]) !=
//           kSimpleSerialOk) {
//         break;
//       }
//       if (*data_len == data_buf_len) {
//         break;
//       }
//       data[*data_len] = hex_byte[0] << 4 | hex_byte[1];
//       ++*data_len;
//     } while (true);
//   }
// }

// /**
//  * Returns the index of a command's handler in `handlers`.
//  *
//  * This function returns the index of the last element, which points to
//  * `simple_serial_unknown_command(), if the given command is not valid.
//  *
//  * @param cmd A simple serial command.
//  * @return Command handler's index in `handlers`.
//  */
// static size_t simple_serial_get_handler_index(uint8_t cmd) {
//   if (simple_serial_is_valid_command(cmd)) {
//     return cmd - 'a';
//   } else {
//     return ARRAYSIZE(handlers) - 1;
//   }
// }

// /**
//  * Simple serial 'v' (version) command handler.
//  *
//  * Returns the simple serial version that this file implements. This command is
//  * useful for checking that the host and the device can communicate properly
//  * before starting capturing traces.
//  *
//  * @param data Received packet payload.
//  * @param data_len Payload length.
//  */
// static void simple_serial_version(const uint8_t *data, size_t data_len) {
//   simple_serial_send_status(kSimpleSerialProtocolVersion);
// }

// /**
//  * Simple serial 's' (seed PRNG) command handler.
//  *
//  * This function only supports 4-byte seeds.
//  *
//  * @param seed A buffer holding the seed.
//  * @param seed_len Seed length.
//  */
// static void simple_serial_seed_prng(const uint8_t *seed, size_t seed_len) {
//   SS_CHECK(seed_len == sizeof(uint32_t));
//   prng_seed(read_32(seed));
// }

// /**
//  * Handler for uninmplemented simple serial commands.
//  *
//  * Sends an error packet over UART.
//  *
//  * @param data Received packet payload
//  * @param data_len Payload length.
//  */
// static void simple_serial_unknown_command(const uint8_t *data,
//                                           size_t data_len) {
//   simple_serial_send_status(kSimpleSerialError);
// }

// void simple_serial_init(const dif_uart_t *uart_) {
//   uart = uart_;

//   for (size_t i = 0; i < ARRAYSIZE(handlers); ++i) {
//     handlers[i] = simple_serial_unknown_command;
//   }
//   handlers[simple_serial_get_handler_index('s')] = simple_serial_seed_prng;
//   handlers[simple_serial_get_handler_index('v')] = simple_serial_version;
// }

// simple_serial_result_t simple_serial_register_handler(
//     uint8_t cmd, simple_serial_command_handler handler) {
//   if (!simple_serial_is_valid_command(cmd)) {
//     return kSimpleSerialError;
//   } else if (cmd == 's' || cmd == 'v') {
//     // Cannot register handlers for built-in commands.
//     return kSimpleSerialError;
//   } else {
//     handlers[simple_serial_get_handler_index(cmd)] = handler;
//     return kSimpleSerialOk;
//   }
// }

// void simple_serial_process_packet(void) {
//   uint8_t cmd;
//   uint8_t data[kUartMaxRxPacketSize];
//   size_t data_len;
//   simple_serial_receive_packet(&cmd, data, ARRAYSIZE(data), &data_len);
//   handlers[simple_serial_get_handler_index(cmd)](data, data_len);
// }

// void simple_serial_send_packet(const uint8_t cmd, const uint8_t *data,
//                                size_t data_len) {
//   char buf;
//   base_snprintf(&buf, 1, "%c", cmd);
//   IGNORE_RESULT(dif_uart_byte_send_polled(uart, buf));
//   simple_serial_print_hex(data, data_len);
//   base_snprintf(&buf, 1, "\n");
//   IGNORE_RESULT(dif_uart_byte_send_polled(uart, buf));
// }

// void simple_serial_send_status(uint8_t res) {
//   simple_serial_send_packet('z', (uint8_t[1]){res}, 1);
// }

// void simple_serial_print_hex(const uint8_t *data, size_t data_len) {
//   char buf[2];
//   for (size_t i = 0; i < data_len; ++i) {
//     base_snprintf(&buf[0], 2, "%2x", data[i]);
//     IGNORE_RESULT(dif_uart_byte_send_polled(uart, buf[0]));
//     IGNORE_RESULT(dif_uart_byte_send_polled(uart, buf[1]));
//   }
// }

// simpleserial.c

#include "simple_serial.h"
#include <stdint.h>
#include "sca.h"


#define MAX_SS_CMDS 16
static int num_commands = 0;

#define MAX_SS_LEN 192

// #define SS_VER SS_VER_2_1

// #define SS_VER_1_0 0
// #define SS_VER_1_1 1
// #define SS_VER_2_0 2


// 0xA6 formerly 
#define CW_CRC 0x4D 
uint8_t ss_crc(uint8_t *buf, uint8_t len)
{
	unsigned int k = 0;
	uint8_t crc = 0x00;
	while (len--) {
		crc ^= *buf++;
		for (k = 0; k < 8; k++) {
			crc = crc & 0x80 ? (crc << 1) ^ CW_CRC: crc << 1;
		}
	}
	return crc;

}

// [B_STUFF, CMD, SCMD, LEN, B_STUFF, DATA..., CRC, TERM]

//#define SS_VER SS_VER_2_0
#if SS_VER == SS_VER_2_0
#error "SS_VER_2_0 is deprecated! Use SS_VER_2_1 instead."
#elif SS_VER == SS_VER_2_1


typedef struct ss_cmd
{
	char c;
	unsigned int len;
	uint8_t (*fp)(uint8_t, uint8_t, uint8_t, uint8_t *);
} ss_cmd;
static ss_cmd commands[MAX_SS_CMDS];

void ss_puts(char *x)
{
	do {
		sca_putch(*x);
	} while (*++x);
}

#define FRAME_BYTE 0x00

uint8_t check_version(uint8_t cmd, uint8_t scmd, uint8_t len, uint8_t *data)
{
	uint8_t ver = SS_VER;
	simpleserial_put('r', 1, &ver);
	return SS_ERR_OK;
}

uint8_t ss_get_commands(uint8_t cmd, uint8_t scmd, uint8_t len, uint8_t *data)
{
    uint8_t cmd_chars[MAX_SS_CMDS];
    for (uint8_t i = 0; i < (num_commands & 0xFF); i++) {
        cmd_chars[i] = commands[i].c;
    }

    simpleserial_put('r', num_commands & 0xFF, (void *)cmd_chars);
    return 0x00;

}

uint8_t stuff_data(uint8_t *buf, uint8_t len)
{
	uint8_t i = 1;
	uint8_t last = 0;
	for (; i < len; i++) {
		if (buf[i] == FRAME_BYTE) {
			buf[last] = i - last;
			last = i;
		}
	}
	return 0x00;
}

uint8_t unstuff_data(uint8_t *buf, uint8_t len)
{
	uint8_t next = buf[0];
	buf[0] = 0x00;
	//len -= 1;
	uint8_t tmp = next;
	while ((next < len) && tmp != 0) {
		tmp = buf[next];
		buf[next] = FRAME_BYTE;
		next += tmp;
	}
	return next;
}

// Set up the SimpleSerial module by preparing internal commands
// This just adds the "v" command for now...
void simpleserial_init()
{
	simpleserial_addcmd('v', 0, check_version);
    simpleserial_addcmd('w', 0, ss_get_commands);
}

int simpleserial_addcmd(char c, unsigned int len, uint8_t (*fp)(uint8_t, uint8_t, uint8_t, uint8_t*))
{
	if(num_commands >= MAX_SS_CMDS) {
		sca_putch('a');
		return 1;
	}

	if(len >= MAX_SS_LEN) {
		sca_putch('b');
		return 1;
	}

	commands[num_commands].c   = c;
	commands[num_commands].len = len;
	commands[num_commands].fp  = fp;
	num_commands++;

	return 0;
}

void simpleserial_get(void)
{
	uint8_t data_buf[MAX_SS_LEN];
	uint8_t err = 0;

	for (int i = 0; i < 4; i++) {
		data_buf[i] = sca_getch(); //PTR, cmd, scmd, len
		if (data_buf[i] == FRAME_BYTE) {
			err = SS_ERR_FRAME_BYTE;
			goto ERROR;
		}
	}
	uint8_t next_frame = unstuff_data(data_buf, 4);

	// check for valid command
	uint8_t c = 0;
	for(c = 0; c < num_commands; c++)
	{
		if(commands[c].c == data_buf[1])
			break;
	}

	if (c == num_commands) {
		err = SS_ERR_CMD;
		goto ERROR;
	}

	//check that next frame not beyond end of message
	// account for cmd, scmd, len, data, crc, end of frame
	if ((data_buf[3] + 5) < next_frame) {
		err = SS_ERR_LEN;
		goto ERROR;
	}

	// read in data
	// eq to len + crc + frame end
	int i = 4;
	for (; i < data_buf[3] + 5; i++) {
		data_buf[i] = sca_getch();
		if (data_buf[i] == FRAME_BYTE) {
			err = SS_ERR_FRAME_BYTE;
			goto ERROR;
		}
	}

	//check that final byte is the FRAME_BYTE
	data_buf[i] = sca_getch();
	if (data_buf[i] != FRAME_BYTE) {
		err = SS_ERR_LEN;
		goto ERROR;
	}

	//fully unstuff data now
	unstuff_data(data_buf + next_frame, i - next_frame + 1);

	//calc crc excluding original frame offset and frame end and crc
	uint8_t crc = ss_crc(data_buf+1, i-2);
	if (crc != data_buf[i-1]) {
		err = SS_ERR_CRC;
		goto ERROR;
	}

	err = commands[c].fp(data_buf[1], data_buf[2], data_buf[3], data_buf+4);

ERROR:
	simpleserial_put('e', 0x01, &err);
	return;
}

void simpleserial_put(char c, uint8_t size, uint8_t* output)
{
	uint8_t data_buf[MAX_SS_LEN];
	data_buf[0] = 0x00;
	data_buf[1] = c;
	data_buf[2] = size;
	int i = 0;
	for (; i < size; i++) {
		data_buf[i + 3] = output[i];
	}
	data_buf[i + 3] = ss_crc(data_buf+1, size+2);
	data_buf[i + 4] = 0x00;
	stuff_data(data_buf, i + 5);
	for (int i = 0; i < size + 5; i++) {
		sca_putch(data_buf[i]);
	}
}


#else

typedef struct ss_cmd
{
	char c;
	unsigned int len;
	uint8_t (*fp)(uint8_t*, uint8_t);
	uint8_t flags;
} ss_cmd;
static ss_cmd commands[MAX_SS_CMDS];
// Callback function for "v" command.
// This can exist in v1.0 as long as we don't actually send back an ack ("z")
uint8_t check_version(uint8_t *v, uint8_t len)
{
	return SS_VER;
}

uint8_t ss_num_commands(uint8_t *x, uint8_t len)
{
    uint8_t ncmds = num_commands & 0xFF;
    simpleserial_put('r', 0x01, &ncmds);
    return 0x00;
}

typedef struct ss_cmd_repr {
    uint8_t c;
    uint8_t len;
    uint8_t flags;
} ss_cmd_repr;

uint8_t ss_get_commands(uint8_t *x, uint8_t len)
{
    ss_cmd_repr repr_cmd_buf[MAX_SS_CMDS];
    for (uint8_t i = 0; i < (num_commands & 0xFF); i++) {
        repr_cmd_buf[i].c = commands[i].c;
        repr_cmd_buf[i].len = commands[i].len;
        repr_cmd_buf[i].flags = commands[i].flags;
    }

    simpleserial_put('r', num_commands * 0x03, (void *) repr_cmd_buf);
    return 0x00;
}

static char hex_lookup[16] =
{
	'0', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

int hex_decode(int len, char* ascii_buf, uint8_t* data_buf)
{
	for(int i = 0; i < len; i++)
	{
		char n_hi = ascii_buf[2*i];
		char n_lo = ascii_buf[2*i+1];

		if(n_lo >= '0' && n_lo <= '9')
			data_buf[i] = n_lo - '0';
		else if(n_lo >= 'A' && n_lo <= 'F')
			data_buf[i] = n_lo - 'A' + 10;
		else if(n_lo >= 'a' && n_lo <= 'f')
			data_buf[i] = n_lo - 'a' + 10;
		else
			return 1;

		if(n_hi >= '0' && n_hi <= '9')
			data_buf[i] |= (n_hi - '0') << 4;
		else if(n_hi >= 'A' && n_hi <= 'F')
			data_buf[i] |= (n_hi - 'A' + 10) << 4;
		else if(n_hi >= 'a' && n_hi <= 'f')
			data_buf[i] |= (n_hi - 'a' + 10) << 4;
		else
			return 1;
	}

	return 0;
}



// Set up the SimpleSerial module by preparing internal commands
// This just adds the "v" command for now...
void simpleserial_init()
{
	simpleserial_addcmd('v', 0, check_version);
    simpleserial_addcmd('w', 0, ss_get_commands);
    simpleserial_addcmd('y', 0, ss_num_commands);
}

int simpleserial_addcmd(char c, unsigned int len, uint8_t (*fp)(uint8_t*, uint8_t))
{
	return simpleserial_addcmd_flags(c, len, fp, CMD_FLAG_NONE);
}

int simpleserial_addcmd_flags(char c, unsigned int len, uint8_t (*fp)(uint8_t*, uint8_t), uint8_t fl)
{
	if(num_commands >= MAX_SS_CMDS)
		return 1;

	if(len >= MAX_SS_LEN)
		return 1;

	commands[num_commands].c   = c;
	commands[num_commands].len = len;
	commands[num_commands].fp  = fp;
	commands[num_commands].flags = fl;
	num_commands++;

	return 0;
}

void simpleserial_get(void)
{
	char ascii_buf[2*MAX_SS_LEN];
	uint8_t data_buf[MAX_SS_LEN];
	char c;

	// Find which command we're receiving
	c = sca_getch();

	int cmd;
	for(cmd = 0; cmd < num_commands; cmd++)
	{
		if(commands[cmd].c == c)
			break;
	}

	// If we didn't find a match, give up right away
	if(cmd == num_commands)
		return;

	// If flag CMD_FLAG_LEN is set, the next byte indicates the sent length
	if ((commands[cmd].flags & CMD_FLAG_LEN) != 0)
	{
		uint8_t l = 0;
		char buff[2];
		buff[0] = sca_getch();
		buff[1] = sca_getch();
		if (hex_decode(1, buff, &l))
			return;
		commands[cmd].len = l;
	}

	// Receive characters until we fill the ASCII buffer
	for(int i = 0; i < 2*commands[cmd].len; i++)
	{
		c = sca_getch();

		// Check for early \n
		if(c == '\n' || c == '\r')
			return;

		ascii_buf[i] = c;
	}

	// Assert that last character is \n or \r
	c = sca_getch();
	if(c != '\n' && c != '\r')
		return;

	// ASCII buffer is full: convert to bytes
	// Check for illegal characters here
	if(hex_decode(commands[cmd].len, ascii_buf, data_buf))
		return;

	// Callback
	uint8_t ret[1];
	ret[0] = commands[cmd].fp(data_buf, commands[cmd].len);

	// Acknowledge (if version is 1.1)
#if SS_VER == SS_VER_1_1
	simpleserial_put('z', 1, ret);
#endif
}

void simpleserial_put(char c, uint8_t size, uint8_t* output)
{
	// Write first character
	sca_putch(c);

	// Write each byte as two nibbles
	for(int i = 0; i < size; i++)
	{
		sca_putch(hex_lookup[output[i] >> 4 ]);
		sca_putch(hex_lookup[output[i] & 0xF]);
	}

	// Write trailing '\n'
	sca_putch('\n');
}

#endif
