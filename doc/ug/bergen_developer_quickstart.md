---
title: "Bergen Board for OpenTitan Developers"
---

![Bergen Board image](../bergen_board/img/bergenk410t.jpeg)

This guide is targeted at existing OpenTitan Developers who want to get up to speed with the Bergen Board.

## Bergen Board Background

Unlike standard dev-boards, Bergen includes a programmable microcontroller that communicates with the target FPGA. This microcontroller is responsible for tasks including:

* Configuring/reconfiguring the FPGA.
* Monitor the FPGA temperature, controlling fans, shutting down power if entering over-temp situation.
* Adjusting the core voltage.
* Controlling the on-board PLL to set required clock frequency.
* Allowing power cycling of the FPGA target.
* USB-serial ports for communication.
* address/data bus which can be used as 30 computer-controller GPIO pins instead.
* Generic SPI interface.

In addition, the board includes several useful features for development of SoC like devices:

* 2x QSPI sockets - one with 1.8V fixed VCC, one with adjustable VCC.
* Standard JTAG headers that are compatible with most Arm & RISC-V debug probes.
* USB with PHY chip (compatible with OT IP).
* PMOD headers
* Spare I/O headers that mate to 0.05" IDC cables.
* DDR3 memory.
* SRAM memory.

The board also contains multiple features specific for power analysis & fault injection testing:

* Multi-stage filtering of VCC-INT power supplies to reduce noise.
* On-board inductive "shunt" for power measurements.
* On-board LNA for improved SNR of power measurements.
* Bridgeable test points for performing other operations with VCC-INT supply.
* Cross-flow fans allow access to die for EM probing or EMFI fault injection.
* SMA connectors for voltage fault injection.

## System Setup

**NB: You will need to get a Vivado 30-day trial license for the K410T device**. This must be done via the online Xilinx website (*not* via the license manager within Vivado). Using the online website will allow you to continue an evaluation by requesting a new evaluation license, as you may not be able to fully evaluate the power of Vivado within 30 days. The license manager within Vivado only allows a single license request.

If you are using a prebuilt bitstream, you will not need to setup Vivado.


### udev rule setup

The Bergen Board uses a new USB device tyoe, so add the following to the `90-opentitan.rules` file in `/etc/udev/rules.d`

```
# ChipWhisperer Bergen Board
ACTION=="add|change", SUBSYSTEM=="usb|tty", ATTRS{idVendor}=="2b3e", ATTRS{idProduct}=="c310", MODE="0666"
```

And reload the rules with `udevadm control --reload-rules` .

### Bergen Loader

The microcontroller communication is done with a Python API that is part of ChipWhisperer standard targets. This API provides full flexibility including features such as setting VCC-INT voltages, power supplies, etc. But a simplified shim program called `bergenloader.py` provides the most likely required features: (1) configuring/reconfiguring the FPGA, and (2) loading software onto the opentitan core using the existing JTAG/SPI interface.

To load a FPGA bitstream for example:

```console
./util/bergenloader.py -bs build/lowrisc_systems_top_earlgrey_bergen-k410t_0.1/synth-vivado/lowrisc_systems_top_earlgrey_bergen-k410t_0.1.bit
```

This will take 5-20 seconds to execute. After, you can simply load software images as such:

```console
./util/bergenloader.py -fw build-bin/sw/device/examples/hello_world/hello_world_fpga_nexysvideo.bin
```

These commands can be combined to run together, in which case the FPGA is always configured first.

### Serial Port Connection

The Bergen Board provides two serial ports for your use. They will normally enumerate as `/dev/ttyACM0` and `/dev/ttyACM1`, although you can check dmesg when plugging in.

In this scenario, the default OpenTitan port is connected to `/dev/ttyACM1`. The required arguments for screen for this serial port to work are:

```console
screen /dev/ttyACM1 115200,cs8,-ixon,-ixoff
```

A lazy script is provided at `./util/bergen-serial.sh` which runs this command. Other options (such as `8n1` instead of `cs8`) don't seem to work, so when setting up please use this command exactly as-is.