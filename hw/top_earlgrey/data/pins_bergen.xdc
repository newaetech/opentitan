# ChipWhisperer CW310 "Bergen Board".
#
# This pin mapping is for the REV06 (final) PCB (and later).
#

## Clock Signal
set_property -dict { PACKAGE_PIN N21    IOSTANDARD LVCMOS33 } [get_ports { IO_CLK }]; # PLL_CLK2
create_clock -add -name sys_clk_pin -period 10.00 -waveform {0 5} [get_ports IO_CLK]

## Clock constraints
## set via clocks.xdc

## Preserve prim_prince modules and setup multi-cycle paths
## These are no longer required, but kept here as a reference
## set_property KEEP_HIERARCHY TRUE [get_cells top_earlgrey/u_flash_eflash/gen_flash_banks[*].i_core/u_scramble/u_cipher]
## set_multicycle_path -setup 2 -through [get_pins -of_objects [get_cells top_earlgrey/u_flash_eflash/gen_flash_banks[*].i_core/u_scramble/u_cipher]]
## set_multicycle_path -hold 1  -through [get_pins -of_objects [get_cells top_earlgrey/u_flash_eflash/gen_flash_banks[*].i_core/u_scramble/u_cipher]]

#set_property CLOCK_DEDICATED_ROUTE FALSE [get_nets IO_SDCK_IBUF]; # SDCK clock to be ignored

## LEDs
set_property -dict { PACKAGE_PIN M26  DRIVE 8 IOSTANDARD LVCMOS33 } [get_ports { IOB2 }];  #LED 0 
set_property -dict { PACKAGE_PIN M25  DRIVE 8 IOSTANDARD LVCMOS33 } [get_ports { IOB3 }];  #LED 1
set_property -dict { PACKAGE_PIN M24  DRIVE 8 IOSTANDARD LVCMOS33 } [get_ports { IOB4 }];  #LED 2
set_property -dict { PACKAGE_PIN M19  DRIVE 8 IOSTANDARD LVCMOS33 } [get_ports { IOB5 }]; #LED 3
set_property -dict { PACKAGE_PIN L25  DRIVE 8 IOSTANDARD LVCMOS33 } [get_ports { IOB6 }]; #LED 4
set_property -dict { PACKAGE_PIN K26  DRIVE 8 IOSTANDARD LVCMOS33 } [get_ports { IOB7 }]; #LED 5
set_property -dict { PACKAGE_PIN L24  DRIVE 8 IOSTANDARD LVCMOS33 } [get_ports { IOB8 }]; #LED 6
set_property -dict { PACKAGE_PIN K25  DRIVE 8 IOSTANDARD LVCMOS33 } [get_ports { IOB9 }]; #LED 7

## Buttons
set_property -dict { PACKAGE_PIN Y7 IOSTANDARD LVCMOS18 } [get_ports { POR_N }]; #pushbutton SW2

## Switches
set_property -dict { PACKAGE_PIN U9 IOSTANDARD LVCMOS18 }  [get_ports { IOA0 }]; #USRDIP0
set_property -dict { PACKAGE_PIN V7 IOSTANDARD LVCMOS18 }  [get_ports { IOA1 }]; #USRDIP1
set_property -dict { PACKAGE_PIN V8 IOSTANDARD LVCMOS18 }  [get_ports { IOA2 }]; #USRDIP2
set_property -dict { PACKAGE_PIN W9 IOSTANDARD LVCMOS18 }  [get_ports { IOA3 }]; #USRDIP3
set_property -dict { PACKAGE_PIN V9 IOSTANDARD LVCMOS18 }  [get_ports { IOA4 }]; #USRDIP4
set_property -dict { PACKAGE_PIN W8 IOSTANDARD LVCMOS18 }  [get_ports { IOA5 }]; #USRDIP5
set_property -dict { PACKAGE_PIN W10 IOSTANDARD LVCMOS18 } [get_ports { IOB0 }]; #USRDIP6
set_property -dict { PACKAGE_PIN V11 IOSTANDARD LVCMOS18 } [get_ports { IOB1 }]; #USRDIP7

## SPI/JTAG
set_property -dict { PACKAGE_PIN D26 IOSTANDARD LVCMOS33 } [get_ports { SPI_DEV_CLK }]; #SCK (SPI1_SCK)
set_property -dict { PACKAGE_PIN A24 IOSTANDARD LVCMOS33 } [get_ports { SPI_DEV_D0 }]; #SDI (SPI1_COPI)
set_property -dict { PACKAGE_PIN A22 IOSTANDARD LVCMOS33 } [get_ports { SPI_DEV_D1 }]; #SDO (SPI1_CIPO)
set_property -dict { PACKAGE_PIN C26 IOSTANDARD LVCMOS33 } [get_ports { SPI_DEV_CS_L }]; #CSB (SPI1_CS)
set_property -dict { PACKAGE_PIN V21 IOSTANDARD LVCMOS33 } [get_ports { IOC0 }]; #JTAG TRST (USB_A17) #SAM3X
set_property -dict { PACKAGE_PIN W21 IOSTANDARD LVCMOS33 PULLTYPE PULLUP } [get_ports { IO_JSRST_N }]; #JTAG SRST (USB_A18)
set_property -dict { PACKAGE_PIN W20 IOSTANDARD LVCMOS33 PULLTYPE PULLDOWN } [get_ports { IOB10 }]; #JTAG/SPI (USB_A19)
set_property -dict { PACKAGE_PIN U21 IOSTANDARD LVCMOS33 PULLTYPE PULLDOWN } [get_ports { IOB11 }]; #Bootstrap (USB_A16)

## OTHER IO
set_property -dict { PACKAGE_PIN A8  IOSTANDARD LVCMOS33 } [get_ports { IOC6 }]; #USERIOB-9
set_property -dict { PACKAGE_PIN B9  IOSTANDARD LVCMOS33 } [get_ports { IOC7 }]; #USERIOB-11
set_property -dict { PACKAGE_PIN A9  IOSTANDARD LVCMOS33 } [get_ports { IOC8 }]; #USERIOB-15
set_property -dict { PACKAGE_PIN E10 IOSTANDARD LVCMOS33 } [get_ports { IOC9 }]; #USERIOB-14
set_property -dict { PACKAGE_PIN D8  IOSTANDARD LVCMOS33 } [get_ports { IOC10 }]; #USERIOB-16
set_property -dict { PACKAGE_PIN D9  IOSTANDARD LVCMOS33 } [get_ports { IOC11 }]; #USERIOB-18
set_property -dict { PACKAGE_PIN C9  IOSTANDARD LVCMOS33 } [get_ports { IOR0 }]; #USERIOB-24
set_property -dict { PACKAGE_PIN D10 IOSTANDARD LVCMOS33 } [get_ports { IOR1 }]; #USERIOB-26

## TI TUSB1106 USB PHY usbdev testing
set_property -dict { PACKAGE_PIN AF19  IOSTANDARD LVCMOS18 } [get_ports { IO_UPHY_DP_TX }]; #USRUSB_VPO
set_property -dict { PACKAGE_PIN AF20  IOSTANDARD LVCMOS18 } [get_ports { IO_UPHY_DN_TX }]; #USRUSB_VMO
set_property -dict { PACKAGE_PIN V16   IOSTANDARD LVCMOS18 } [get_ports { IO_UPHY_DP_RX }]; #USRUSB_VP
set_property -dict { PACKAGE_PIN V13   IOSTANDARD LVCMOS18 } [get_ports { IO_UPHY_DN_RX }]; #USRUSB_VM
set_property -dict { PACKAGE_PIN AF14  IOSTANDARD LVCMOS18 } [get_ports { IO_UPHY_DPPULLUP }]; #USRUSB_SOFTCONN
set_property -dict { PACKAGE_PIN AE15  IOSTANDARD LVCMOS18 } [get_ports { IO_UPHY_OE_N }]; #USRUSB_OE
set_property -dict { PACKAGE_PIN V17   IOSTANDARD LVCMOS18 } [get_ports { IO_UPHY_D_RX }]; #USRUSB_RCV
set_property -dict { PACKAGE_PIN AE16  IOSTANDARD LVCMOS18 } [get_ports { IO_UPHY_SPD  }]; #USRUSB_SPD
set_property -dict { PACKAGE_PIN AF15  IOSTANDARD LVCMOS18 } [get_ports { IO_UPHY_SUS  }]; #USRUSB_SUS
set_property -dict { PACKAGE_PIN P18   IOSTANDARD LVCMOS33 } [get_ports { IO_UPHY_SENSE }]; #USRUSB_VBUS_DETECT

#Not used - route to header for now?
set_property -dict { PACKAGE_PIN A10   IOSTANDARD LVCMOS33 } [get_ports { USB_P }]; #USERIOB-19
set_property -dict { PACKAGE_PIN B11   IOSTANDARD LVCMOS33 } [get_ports { USB_N }]; #USERIOB-21
set_property -dict { PACKAGE_PIN B12   IOSTANDARD LVCMOS33 } [get_ports { IO_USB_SENSE0 }]; #USERIOB-23
set_property -dict { PACKAGE_PIN A12   IOSTANDARD LVCMOS33 } [get_ports { IO_USB_DNPULLUP0 }]; #USERIOB-25
set_property -dict { PACKAGE_PIN A13   IOSTANDARD LVCMOS33 } [get_ports { IO_USB_DPPULLUP0 }]; #USERIOB-27

## UART
set_property -dict { PACKAGE_PIN AA22 IOSTANDARD LVCMOS33 } [get_ports { IOR3 }]; #UART1RXD
set_property -dict { PACKAGE_PIN W24  IOSTANDARD LVCMOS33 } [get_ports { IOR2 }]; #UART1TXD

## Configuration options, can be used for all designs
set_property CONFIG_VOLTAGE 3.3 [current_design]
set_property CFGBVS VCCO [current_design]
