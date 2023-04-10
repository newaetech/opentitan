## Copyright lowRISC contributors.
## Licensed under the Apache License, Version 2.0, see LICENSE for details.
## SPDX-License-Identifier: Apache-2.0

## ChipWhisperer CW341 "Bergen Board".
##

## Clock Signal
# PLL_CLK2 should be AN29, but here D23 is used /USB_CE so requires a jumper. Using AN29 caused placement error which could be worked
# around, but for now use this pin, consider changing pinout on hardware.
set_property -dict { PACKAGE_PIN D23 IOSTANDARD LVCMOS18 } [get_ports { IO_CLK }];

## Clock constraints
## set via clocks.xdc

## Buttons
set_property -dict { PACKAGE_PIN AN13 IOSTANDARD LVCMOS18 PULLTYPE PULLUP} [get_ports { POR_BUTTON_N }]; # Connects to PMOD, this should actually be
                                                                                                         # NC as there is no use of this on CW340 now.

## JTAG system reset
set_property -dict { PACKAGE_PIN  E23  IOSTANDARD LVCMOS18 } [get_ports { JTAG_SRST_N }]; #USR_DBG_nRST (nSRST) #removed PULLTYPE PULLUP for FTDI usage

## Power-on Reset
set_property -dict { PACKAGE_PIN  AC31 IOSTANDARD LVCMOS18 PULLTYPE PULLUP } [get_ports { POR_N }]; # Main PORN, requires jumper to SW1 [was USB_A14 (SAM3X)]

## Preserve prim_prince modules and setup multi-cycle paths
## These are no longer required, but kept here as a reference
## set_property KEEP_HIERARCHY TRUE [get_cells top_earlgrey/u_flash_eflash/gen_flash_banks[*].i_core/u_scramble/u_cipher]
## set_multicycle_path -setup 2 -through [get_pins -of_objects [get_cells top_earlgrey/u_flash_eflash/gen_flash_banks[*].i_core/u_scramble/u_cipher]]
## set_multicycle_path -hold 1  -through [get_pins -of_objects [get_cells top_earlgrey/u_flash_eflash/gen_flash_banks[*].i_core/u_scramble/u_cipher]]

#set_property CLOCK_DEDICATED_ROUTE FALSE [get_nets IO_SDCK_IBUF]; # SDCK clock to be ignored


## IOA bank
# UART (SAM3X)
set_property -dict { PACKAGE_PIN AN26 IOSTANDARD LVCMOS18 } [get_ports { IOA0 }]; # SAM_TXD0 - OpenTitan UART2 RX
set_property -dict { PACKAGE_PIN AK26 IOSTANDARD LVCMOS18 } [get_ports { IOA1 }]; # SAM_RXD0 - OpenTitan UART2 TX
# GPIOs (PMOD1)
set_property -dict { PACKAGE_PIN AN27  IOSTANDARD LVCMOS18 } [get_ports { IOA2 }]; # PMOD1_IO1 (OT CTS)
set_property -dict { PACKAGE_PIN AP26  IOSTANDARD LVCMOS18 } [get_ports { IOA3 }]; # PMOD1_IO4 (OT RTS)
# UART (PMOD1)
set_property -dict { PACKAGE_PIN AP28  IOSTANDARD LVCMOS18 } [get_ports { IOA4 }]; # PMOD1_IO3 (OT RXD)
set_property -dict { PACKAGE_PIN AM27  IOSTANDARD LVCMOS18 } [get_ports { IOA5 }]; # PMOD1_IO2 (OT TXD)
# GPIO (PMOD1)
set_property -dict { PACKAGE_PIN AP29  IOSTANDARD LVCMOS18 } [get_ports { IOA6 }]; # PMOD1_IO5 (INT)
# I2C (PMOD1)
set_property -dict { PACKAGE_PIN AN28  IOSTANDARD LVCMOS18 } [get_ports { IOA7 }]; # PMOD1_IO7 (SCL)
set_property -dict { PACKAGE_PIN AP30  IOSTANDARD LVCMOS18 }  [get_ports { IOA8 }]; # PMOD1_IO8 (SDA)

## IOB bank
# Aux SPI host
set_property -dict { PACKAGE_PIN AP8  IOSTANDARD LVCMOS18 } [get_ports { IOB0 }]; # USR_SPI1CLK
set_property -dict { PACKAGE_PIN AN8  IOSTANDARD LVCMOS18 } [get_ports { IOB1 }]; # USR_SPI1DQ0
set_property -dict { PACKAGE_PIN AP9  IOSTANDARD LVCMOS18 } [get_ports { IOB2 }]; # USR_SPI1DQ1
set_property -dict { PACKAGE_PIN AL8  IOSTANDARD LVCMOS18 } [get_ports { IOB3 }]; # USR_SPI1CS
# ChipWhisperer UART
set_property -dict { PACKAGE_PIN AL10 IOSTANDARD LVCMOS18 } [get_ports { IOB4 }]; #J14 PIN 10 CWIO_IO1 - OpenTitan UART1 RX
set_property -dict { PACKAGE_PIN AN9  IOSTANDARD LVCMOS18 } [get_ports { IOB5 }]; #J14 PIN 12 CWIO_IO2 - OpenTitan UART1 TX
# GPIOs (DIP switches)
set_property -dict { PACKAGE_PIN AM10 IOSTANDARD LVCMOS18 } [get_ports { IOB6  }]; # USRDIP0
set_property -dict { PACKAGE_PIN AP10 IOSTANDARD LVCMOS18 } [get_ports { IOB7  }]; # USRDIP1
set_property -dict { PACKAGE_PIN AL9  IOSTANDARD LVCMOS18 } [get_ports { IOB8  }]; # USRDIP2
set_property -dict { PACKAGE_PIN AP11 IOSTANDARD LVCMOS18 } [get_ports { IOB9  }]; # USRDIP3
set_property -dict { PACKAGE_PIN AM12 IOSTANDARD LVCMOS18 } [get_ports { IOB10 }]; # USRDIP4
set_property -dict { PACKAGE_PIN AN11 IOSTANDARD LVCMOS18 } [get_ports { IOB11 }]; # USRDIP5
set_property -dict { PACKAGE_PIN AN12 IOSTANDARD LVCMOS18 } [get_ports { IOB12 }]; # USRDIP6


## IOC bank
# SW Straps
set_property -dict { PACKAGE_PIN AG34  IOSTANDARD LVCMOS18 } [get_ports { IOC0 }]; #USB_A15 (SAM3X) #removed PULLTYPE PULLDOWN for FTDI usage
set_property -dict { PACKAGE_PIN AF34  IOSTANDARD LVCMOS18 } [get_ports { IOC1 }]; #USB_A16 (SAM3X) #removed PULLTYPE PULLDOWN for FTDI usage
set_property -dict { PACKAGE_PIN AF33  IOSTANDARD LVCMOS18 } [get_ports { IOC2 }]; #USB_A17 (SAM3X) #removed PULLTYPE PULLDOWN for FTDI usage
# Main UART
set_property -dict { PACKAGE_PIN AE33  IOSTANDARD LVCMOS18 } [get_ports { IOC3 }]; #UART1TXD - OpenTitan UART0 RX
set_property -dict { PACKAGE_PIN AD34 IOSTANDARD LVCMOS18 } [get_ports { IOC4 }]; #UART1RXD - OpenTitan UART0 TX
# TAP Strap 1
set_property -dict { PACKAGE_PIN AC34  IOSTANDARD LVCMOS18 } [get_ports { IOC5 }]; #USB_A19 (SAM3X) #removed PULLTYPE PULLDOWN for FTDI usage
# PWM (PMOD2)
set_property -dict { PACKAGE_PIN AB34  IOSTANDARD LVCMOS18 } [get_ports { IOC6 }]; # PMOD2_IO2 (PWM)
# USB VBUS Detection
set_property -dict { PACKAGE_PIN AA34  IOSTANDARD LVCMOS18 } [get_ports { IOC7 }]; # USRUSB_VBUS_DETECT
# TAP Strap 0
set_property -dict { PACKAGE_PIN AB32  IOSTANDARD LVCMOS18 } [get_ports { IOC8 }]; #USB_A18 (SAM3X) #removed PULLTYPE PULLDOWN for FTDI usage
# GPIOs (PMOD2)
set_property -dict { PACKAGE_PIN AB31 IOSTANDARD LVCMOS18 } [get_ports { IOC9  }]; # PMOD2_IO1 (GPIO)
set_property -dict { PACKAGE_PIN AA32 IOSTANDARD LVCMOS18 } [get_ports { IOC10 }]; # PMOD2_IO3 (GPIO)
set_property -dict { PACKAGE_PIN V34 IOSTANDARD LVCMOS18 } [get_ports { IOC11 }]; # PMOD2_IO4 (GPIO)
# GPIO (PMOD1)
set_property -dict { PACKAGE_PIN U34 IOSTANDARD LVCMOS18 } [get_ports { IOC12 }]; # PMOD1_IO6 (RESET)

## IOR bank
# JTAG
# ultrascale PU current is 60 to 678uA, = 30K to 2.6K
set_property -dict { PACKAGE_PIN AL33 IOSTANDARD LVCMOS18 } [get_ports { IOR0  }]; #USR_DBG_TMS #removed PULLTYPE PULLUP for FTDI usage
set_property -dict { PACKAGE_PIN AK27 IOSTANDARD LVCMOS18 } [get_ports { IOR1  }]; #USR_DBG_TDO #removed PULLTYPE PULLUP for FTDI usage
set_property -dict { PACKAGE_PIN AK31 IOSTANDARD LVCMOS18 } [get_ports { IOR2  }]; #USR_DBG_TDI #removed PULLTYPE PULLUP for FTDI usage
set_property -dict { PACKAGE_PIN AL34 IOSTANDARD LVCMOS18 } [get_ports { IOR3  }]; #USR_DBG_TCK #removed PULLTYPE PULLUP for FTDI usage
set_property -dict { PACKAGE_PIN AJ34 IOSTANDARD LVCMOS18 } [get_ports { IOR4  }]; #USR_DBG_TDAT1 (nTRST) #removed PULLTYPE PULLUP for FTDI usage
# GPIO (LED)
set_property -dict { PACKAGE_PIN AK30 IOSTANDARD LVCMOS18 } [get_ports { IOR5  }]; # USRDIP7
set_property -dict { PACKAGE_PIN AH32 DRIVE 8 IOSTANDARD LVCMOS18 } [get_ports { IOR6  }]; # USRLED0
set_property -dict { PACKAGE_PIN AJ33 DRIVE 8 IOSTANDARD LVCMOS18 } [get_ports { IOR7  }]; # USRLED1
set_property -dict { PACKAGE_PIN AH34 DRIVE 8 IOSTANDARD LVCMOS18 } [get_ports { IOR8  }]; # USRLED2
set_property -dict { PACKAGE_PIN AH31 DRIVE 8 IOSTANDARD LVCMOS18 } [get_ports { IOR9  }]; # USRLED3
set_property -dict { PACKAGE_PIN AH27 DRIVE 8 IOSTANDARD LVCMOS18 } [get_ports { IOR10 }]; # USRLED4
set_property -dict { PACKAGE_PIN AH33 DRIVE 8 IOSTANDARD LVCMOS18 } [get_ports { IOR11 }]; # USRLED5
set_property -dict { PACKAGE_PIN AH28 DRIVE 8 IOSTANDARD LVCMOS18 } [get_ports { IOR12 }]; # USRLED6
set_property -dict { PACKAGE_PIN AH26 DRIVE 8 IOSTANDARD LVCMOS18 } [get_ports { IOR13 }]; # USRLED7

## SPI device
set_property -dict { PACKAGE_PIN AM30 IOSTANDARD LVCMOS18 } [get_ports { SPI_DEV_CLK  }]; #SCK (SPI1_SCK)
set_property -dict { PACKAGE_PIN AL30 IOSTANDARD LVCMOS18 } [get_ports { SPI_DEV_D0   }]; #SDI (SPI1_COPI)
set_property -dict { PACKAGE_PIN AM26 IOSTANDARD LVCMOS18 } [get_ports { SPI_DEV_D1   }]; #SDO (SPI1_CIPO)
set_property -dict { PACKAGE_PIN AN32 IOSTANDARD LVCMOS18 } [get_ports { SPI_DEV_D2   }]; #
set_property -dict { PACKAGE_PIN AN34 IOSTANDARD LVCMOS18 } [get_ports { SPI_DEV_D3   }]; #
set_property -dict { PACKAGE_PIN AM34 IOSTANDARD LVCMOS18 } [get_ports { SPI_DEV_CS_L }]; #CSB (SPI1_CS)

## SPI HOST
set_property -dict { PACKAGE_PIN AP31 IOSTANDARD LVCMOS18 } [get_ports { SPI_HOST_CLK }];   #SCK (USR_SPI0CLK)
set_property -dict { PACKAGE_PIN AP33 IOSTANDARD LVCMOS18 PULLTYPE PULLUP } [get_ports { SPI_HOST_D0 }]; #SDO (USR_SPI0DQ0)
set_property -dict { PACKAGE_PIN AP34 IOSTANDARD LVCMOS18 PULLTYPE PULLUP } [get_ports { SPI_HOST_D1 }]; #SDO (USR_SPI0DQ1)
set_property -dict { PACKAGE_PIN AL27 IOSTANDARD LVCMOS18 PULLTYPE PULLUP } [get_ports { SPI_HOST_D2 }]; #SDO (USR_SPI0DQ2)
set_property -dict { PACKAGE_PIN AN33 IOSTANDARD LVCMOS18 PULLTYPE PULLUP } [get_ports { SPI_HOST_D3 }]; #SDO (USR_SPI0DQ3)
set_property -dict { PACKAGE_PIN AL32 IOSTANDARD LVCMOS18 } [get_ports { SPI_HOST_CS_L }]; #CSB (USR_SPI0CS)

## ChipWhisperer 20-Pin Connector (J14)
set_property -dict { PACKAGE_PIN AM11 IOSTANDARD LVCMOS18 } [get_ports { IO_TRIGGER }]; #Jn PIN 16 CWIO_IO4 - Capture Trigger
set_property -dict { PACKAGE_PIN AK12 IOSTANDARD LVCMOS18 } [get_ports { IO_CLKOUT }];  #Jn PIN  4 CWIO_HS1 - Target clock

## TI TUSB1106 USB PHY usbdev testing
set_property -dict { PACKAGE_PIN E28  IOSTANDARD LVCMOS18 } [get_ports { IO_USB_DP_TX }]; #USRUSB_VPO
set_property -dict { PACKAGE_PIN A27  IOSTANDARD LVCMOS18 } [get_ports { IO_USB_DN_TX }]; #USRUSB_VMO
set_property -dict { PACKAGE_PIN C27  IOSTANDARD LVCMOS18 } [get_ports { IO_USB_DP_RX }]; #USRUSB_VP
set_property -dict { PACKAGE_PIN B27  IOSTANDARD LVCMOS18 } [get_ports { IO_USB_DN_RX }]; #USRUSB_VM
set_property -dict { PACKAGE_PIN E27  IOSTANDARD LVCMOS18 } [get_ports { IO_USB_CONNECT }]; #USRUSB_SOFTCONN
set_property -dict { PACKAGE_PIN D24  IOSTANDARD LVCMOS18 } [get_ports { IO_USB_OE_N }]; #USRUSB_OE
set_property -dict { PACKAGE_PIN F27  IOSTANDARD LVCMOS18 } [get_ports { IO_USB_D_RX }]; #USRUSB_RCV
set_property -dict { PACKAGE_PIN A28  IOSTANDARD LVCMOS18 } [get_ports { IO_USB_SPEED }]; #USRUSB_SPD
set_property -dict { PACKAGE_PIN A25  IOSTANDARD LVCMOS18 } [get_ports { IO_USB_SUSPEND }]; #USRUSB_SUS

## USB input delay to accommodate T_FST (full-speed transition time) and the
## PHY's sampling logic. The PHY expects to only see up to one transient / fake
## SE0. The phase relationship with the PHY's sampling clock is arbitrary, but
## for simplicity, constrain the maximum path delay to something smaller than
## `T_sample - T_FST(max)` to help keep the P/N skew from slipping beyond one
## sample period.
set clks_48_unbuf [get_clocks -of_objects [get_pin clkgen/pll/CLKOUT1]]
set_input_delay -clock ${clks_48_unbuf} -min 3 [get_ports {IO_USB_DP_RX IO_USB_DN_RX IO_USB_D_RX}]
set_input_delay -clock ${clks_48_unbuf} -add_delay -max 17 [get_ports {IO_USB_DP_RX IO_USB_DN_RX IO_USB_D_RX}]

## USB output max skew constraint
## Use the output-enable as a "clock" and time the P/N relative to it. Keep the skew within T_FST.
set usb_embed_out_clk [create_generated_clock -name usb_embed_out_clk -source [get_pin clkgen/pll/CLKOUT1] -multiply_by 1 [get_ports IO_USB_OE_N]]
set_false_path -from [get_clocks -include_generated_clocks clk_io_div4] -to ${usb_embed_out_clk}
set_output_delay -min -clock ${usb_embed_out_clk} 7 [get_ports {IO_USB_DP_TX IO_USB_DN_TX}]
set_output_delay -max -clock ${usb_embed_out_clk} 14 [get_ports {IO_USB_DP_TX IO_USB_DN_TX}] -add_delay


## Configuration options, can be used for all designs
set_property CONFIG_VOLTAGE 1.8 [current_design]
set_property CFGBVS GND [current_design]
