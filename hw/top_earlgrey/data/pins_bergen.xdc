# ChipWhisperer CW310 "Bergen Board".
#
# WARNING WARNING WARNING - This XDC file is for "rev04" only. The pin mapping changes in the final (rev05 and later) release.
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
set_property -dict { PACKAGE_PIN R16  DRIVE 8 IOSTANDARD LVCMOS33 } [get_ports { IO_GP8 }];  #PMOD2-1
set_property -dict { PACKAGE_PIN R17  DRIVE 8 IOSTANDARD LVCMOS33 } [get_ports { IO_GP9 }];  #PMOD2-2
set_property -dict { PACKAGE_PIN N18  DRIVE 8 IOSTANDARD LVCMOS33 } [get_ports { IO_GP10}]; #PMOD2-3

## Buttons
set_property -dict { PACKAGE_PIN AB10 IOSTANDARD LVCMOS18 } [get_ports { IO_RST_N }]; #pushbutton

## Switches
set_property -dict { PACKAGE_PIN N18 IOSTANDARD LVCMOS33 } [get_ports { IO_GP0 }]; #PMOD2-4
set_property -dict { PACKAGE_PIN N16 IOSTANDARD LVCMOS33 } [get_ports { IO_GP1 }]; #PMOD2-5
set_property -dict { PACKAGE_PIN T19 IOSTANDARD LVCMOS33 } [get_ports { IO_GP2 }]; #PMOD2-6
set_property -dict { PACKAGE_PIN P18 IOSTANDARD LVCMOS33 } [get_ports { IO_GP3 }]; #PMOD2-7

## SPI/JTAG
set_property -dict { PACKAGE_PIN R26 IOSTANDARD LVCMOS33 } [get_ports { IO_DPS0 }]; #SCK (SPI1_SCK)
set_property -dict { PACKAGE_PIN R25 IOSTANDARD LVCMOS33 } [get_ports { IO_DPS1 }]; #SDI (SPI1_COPI)
set_property -dict { PACKAGE_PIN N26 IOSTANDARD LVCMOS33 } [get_ports { IO_DPS2 }]; #SDO (SPI1_CIPO)
set_property -dict { PACKAGE_PIN T24 IOSTANDARD LVCMOS33 } [get_ports { IO_DPS3 }]; #CSB (SPI1_CS)
set_property -dict { PACKAGE_PIN F24 IOSTANDARD LVCMOS33 } [get_ports { IO_DPS4 }]; #JTAG TRST (USB_A17) #SAM3X
set_property -dict { PACKAGE_PIN C24 IOSTANDARD LVCMOS33 PULLTYPE PULLUP } [get_ports { IO_DPS5 }]; #JTAG SRST (USB_A18)
set_property -dict { PACKAGE_PIN L23 IOSTANDARD LVCMOS33 PULLTYPE PULLDOWN } [get_ports { IO_DPS6 }]; #JTAG/SPI (USB_A19)
set_property -dict { PACKAGE_PIN G24 IOSTANDARD LVCMOS33 PULLTYPE PULLDOWN } [get_ports { IO_DPS7 }]; #Bootstrap (USB_A16)

## OTHER IO
set_property -dict { PACKAGE_PIN C12 IOSTANDARD LVCMOS33 } [get_ports { IO_GP4  }]; #USERIOB-1
set_property -dict { PACKAGE_PIN D13 IOSTANDARD LVCMOS33 } [get_ports { IO_GP5  }]; #USERIOB-3
set_property -dict { PACKAGE_PIN C13 IOSTANDARD LVCMOS33 } [get_ports { IO_GP6  }]; #USERIOB-5
set_property -dict { PACKAGE_PIN E13 IOSTANDARD LVCMOS33 } [get_ports { IO_GP7  }]; #USERIOB-7

set_property -dict { PACKAGE_PIN B10 IOSTANDARD LVCMOS33 } [get_ports { IO_GP11 }]; #USERIOB-17
set_property -dict { PACKAGE_PIN F10 IOSTANDARD LVCMOS33 } [get_ports { IO_GP12 }]; #USERIOB-6
set_property -dict { PACKAGE_PIN H8  IOSTANDARD LVCMOS33 } [get_ports { IO_GP13 }]; #USERIOB-8
set_property -dict { PACKAGE_PIN F8  IOSTANDARD LVCMOS33 } [get_ports { IO_GP14 }]; #USERIOB-10
set_property -dict { PACKAGE_PIN F9  IOSTANDARD LVCMOS33 } [get_ports { IO_GP15 }]; #USERIOB-12

set_property -dict { PACKAGE_PIN A8  IOSTANDARD LVCMOS33 } [get_ports { IO_GP24 }]; #USERIOB-9
set_property -dict { PACKAGE_PIN B9  IOSTANDARD LVCMOS33 } [get_ports { IO_GP25 }]; #USERIOB-11
set_property -dict { PACKAGE_PIN A9  IOSTANDARD LVCMOS33 } [get_ports { IO_GP26 }]; #USERIOB-15
set_property -dict { PACKAGE_PIN E10 IOSTANDARD LVCMOS33 } [get_ports { IO_GP27 }]; #USERIOB-14
set_property -dict { PACKAGE_PIN D8  IOSTANDARD LVCMOS33 } [get_ports { IO_GP28 }]; #USERIOB-16
set_property -dict { PACKAGE_PIN D9  IOSTANDARD LVCMOS33 } [get_ports { IO_GP29 }]; #USERIOB-18
set_property -dict { PACKAGE_PIN C9  IOSTANDARD LVCMOS33 } [get_ports { IO_GP30 }]; #USERIOB-24
set_property -dict { PACKAGE_PIN D10 IOSTANDARD LVCMOS33 } [get_ports { IO_GP31 }]; #USERIOB-26


## TI TUSB1106 USB PHY usbdev testing
set_property -dict { PACKAGE_PIN P16   IOSTANDARD LVCMOS33 } [get_ports { IO_UPHY_DP_TX }]; #USRUSB_VPO
set_property -dict { PACKAGE_PIN N17   IOSTANDARD LVCMOS33 } [get_ports { IO_UPHY_DN_TX }]; #USRUSB_VMO
set_property -dict { PACKAGE_PIN U20   IOSTANDARD LVCMOS33 } [get_ports { IO_UPHY_DP_RX }]; #USRUSB_VP
set_property -dict { PACKAGE_PIN U19   IOSTANDARD LVCMOS33 } [get_ports { IO_UPHY_DN_RX }]; #USRUSB_VM
set_property -dict { PACKAGE_PIN T17   IOSTANDARD LVCMOS33 } [get_ports { IO_UPHY_DPPULLUP }]; #USRUSB_SOFTCONN
set_property -dict { PACKAGE_PIN U16   IOSTANDARD LVCMOS33 } [get_ports { IO_UPHY_SENSE }]; #USRUSB_VBUS_DETECT
set_property -dict { PACKAGE_PIN U17   IOSTANDARD LVCMOS33 } [get_ports { IO_UPHY_OE_N }]; #USRUSB_OE
set_property -dict { PACKAGE_PIN T18   IOSTANDARD LVCMOS33 } [get_ports { IO_UPHY_D_RX }]; #USRUSB_RCV
set property -dict { PACKAGE_PIN M19   IOSTANDARD LVCMOS33 } [get_ports { IO_UPHY_SPD  }]; #USRUSB_SPD
set property -dict { PACKAGE_PIN R18   IOSTANDARD LVCMOS33 } [get_ports { IO_UPHY_SUS  }]; #USRUSB_SUS


set_property -dict { PACKAGE_PIN A10   IOSTANDARD LVCMOS33 } [get_ports { IO_USB_DP0 }]; #USERIOB-19
set_property -dict { PACKAGE_PIN B11   IOSTANDARD LVCMOS33 } [get_ports { IO_USB_DN0 }]; #USERIOB-21
set_property -dict { PACKAGE_PIN B12   IOSTANDARD LVCMOS33 } [get_ports { IO_USB_SENSE0 }]; #USERIOB-23
set_property -dict { PACKAGE_PIN A12   IOSTANDARD LVCMOS33 } [get_ports { IO_USB_DNPULLUP0 }]; #USERIOB-25
set_property -dict { PACKAGE_PIN A13   IOSTANDARD LVCMOS33 } [get_ports { IO_USB_DPPULLUP0 }]; #USERIOB-27

## UART
set_property -dict { PACKAGE_PIN P26  IOSTANDARD LVCMOS33 } [get_ports { IO_UTX }]; #UART1
set_property -dict { PACKAGE_PIN P25  IOSTANDARD LVCMOS33 } [get_ports { IO_URX }]; #UART1

## SPI/JTAG
set_property -dict { PACKAGE_PIN R26 IOSTANDARD LVCMOS33 } [get_ports { IO_DPS0 }]; #SCK (SPI1_SCK)
set_property -dict { PACKAGE_PIN R25 IOSTANDARD LVCMOS33 } [get_ports { IO_DPS1 }]; #SDI (SPI1_COPI)
set_property -dict { PACKAGE_PIN N26 IOSTANDARD LVCMOS33 } [get_ports { IO_DPS2 }]; #SDO (SPI1_CIPO)
set_property -dict { PACKAGE_PIN T24 IOSTANDARD LVCMOS33 } [get_ports { IO_DPS3 }]; #CSB (SPI1_CS)
set_property -dict { PACKAGE_PIN F24 IOSTANDARD LVCMOS33 } [get_ports { IO_DPS4 }]; #JTAG TRST (USB_A17) #SAM3X
set_property -dict { PACKAGE_PIN C24 IOSTANDARD LVCMOS33 PULLTYPE PULLUP } [get_ports { IO_DPS5 }]; #JTAG SRST (USB_A18)
set_property -dict { PACKAGE_PIN L23 IOSTANDARD LVCMOS33 PULLTYPE PULLDOWN } [get_ports { IO_DPS6 }]; #JTAG/SPI (USB_A19)
set_property -dict { PACKAGE_PIN G24 IOSTANDARD LVCMOS33 PULLTYPE PULLDOWN } [get_ports { IO_DPS7 }]; #Bootstrap (USB_A16)


## Configuration options, can be used for all designs
set_property CONFIG_VOLTAGE 3.3 [current_design]
set_property CFGBVS VCCO [current_design]
