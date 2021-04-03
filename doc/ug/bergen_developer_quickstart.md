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

If you are using a prebuilt bitstream, you will not need to setup Vivado (see section [OpenTitan Bergen Board Examples](#OpenTitan Bergen Board Examples)).


### udev rule setup

The Bergen Board uses a new USB device type, so add the following to the `90-opentitan.rules` file in `/etc/udev/rules.d` which will also allow you access to other newae devices:

```
# ChipWhisperer Bergen Board
ACTION=="add|change", SUBSYSTEM=="usb|tty", ATTRS{idVendor}=="2b3e", ATTRS{idProduct}=="ace[0-9]|c[3-6][0-9][0-9]", MODE="0666"
```

And reload the rules with `udevadm control --reload-rules` .

If you'll be using the `usb` example OpenTitan code you might want to also add:

```
# OpenTitan USB serial interface for testing
ACTION=="add|change", SUBSYSTEM=="usb|tty", ATTRS{idVendor}=="18d1", ATTRS{idProduct}=="503a", MODE="0666"
```

### ChipWhisperer Installation

The BergenLoader relies on ChipWhisperer - this is available via `pip`, but we want to track the development branch due to recent changes in the CW310 support instead. For now:

```console
cd $somewhere
git clone https://github.com/newaetech/chipwhisperer
cd chipwhisperer
python setup.py develop --user
```

Note this is suggesting to use `develop` so you can just do a `git pull` in the `chipwhisperer` directory.


## Talking to Bergen Board

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

## OpenTitan Bergen Board Examples

You can easily build some of the examples for the Bergen Board. For example the normal build process for the Bergen Board becomes:

```console
$ . /tools/xilinx/Vivado/2020.1/settings64.sh
$ cd $REPO_TOP
$ ./meson_init.sh
$ ninja -C build-out all
$ fusesoc --cores-root . run --flag=fileset_top --target=synth lowrisc:systems:top_earlgrey_bergen-k410t
```

Note the following:

* There is no need to run the `top_earlygrey_reduce` script for this build.
* The board is specified as `bergen-k410t`, since there are two sizes of FPGA allowed on the boards.

You could then load the examples as above using the `bergenloader` utility:

```console
./util/bergenloader.py -bs build/lowrisc_systems_top_earlgrey_bergen-k410t_0.1/synth-vivado/lowrisc_systems_top_earlgrey_bergen-k410t_0.1.bit -fw build-bin/sw/device/examples/hello_world/hello_world_fpga_nexysvideo.bin
```

### Running Provided Software

Premade binaries are provided in the branch currently. For example the hello-world is available as:

```console
cd $REPO_TOP
./util/bergenloader.py -bs temp_build_cache/lowrisc_systems_top_earlgrey_bergen-k410t_0.1.bit -fw temp_build_cache/hello_world_fpga_nexysvideo.bin
```

To view the output of this, run the serial command as:

```console
cd $REPO_TOP
sh util/bergen-serial.sh
```
*NOTE FOR LATER: When adding this to newbie guide, add note about 'Use `Ctrl-A` then `/` to exit screen'*

## OT-Server Walk-Through

The following is the ultra-quick start guide for using the OT-Server with attached Bergen Board.

With this system, Vivado & most system utilities are already installed (all of the `udev` settings and additional software mentioned earlier included). You will need to install some tools (python packages mostly) to your user directory.

The following commands show checking out the example code and installing the required tooling. Note this tracks the `newaetech/opentitan:hw_cw310` repo as it supports the CW310 "Bergen Board".

```console
cd ~
git clone https://github.com/newaetech/chipwhisperer.git
cd chipwhisperer
pip3 install --user scipy
python3 setup.py develop --user
cd ~
git clone https://github.com/newaetech/opentitan.git
cd opentitan
export REPO_TOP=`pwd`
cd $REPO_TOP
git checkout hw_cw310
pip3 install --user -r python-requirements.txt
```

Note that auto detach has been disabled in user accounts for screen with:

```
echo autodetach off >> ~/.screenrc
```

This is to avoid serial port conflicts when you disconnect (or time out) and your screen session stays alive but detached, preventing others from using the serial port.

### Hello World from OT Server

Now you could for example run `hello_world` demo. First attach a serial port in one console:

```console
cd $REPO_TOP
sh util/bergen-serial.sh
```

Then in another session/console:

```console
cd $REPO_TOP
./util/bergenloader.py -bs temp_build_cache/lowrisc_systems_top_earlgrey_bergen-k410t_0.1.bit
```

This will take 5-20 seconds. It should output a message such as this during operation:

```
Bergen Loader: Attemping to find CW310 Bergen Board:
    Using bitstream :temp_build_cache/lowrisc_systems_top_earlgrey_bergen-k410t_0.1.bit
Board found, setting PLL2 to 100 MHz
Loading done.
```

You should see some messages in the serial console, but it often seems to need a reset which we'll do next. The easiest is to run the following command next which loads the software image & resets the board cleanly:

```console
./util/bergenloader.py -fw temp_build_cache/hello_world_fpga_nexysvideo.bin
```

You should see this message from the bergenloader:

```
Bergen Loader: Attemping to find CW310 Bergen Board:
    No bitstream specified
Board found, setting PLL2 to 100 MHz
INFO: Programming firmware file: temp_build_cache/hello_world_fpga_nexysvideo.bin
Programming OpenTitan with "temp_build_cache/hello_world_fpga_nexysvideo.bin"...
Transferring frame 0x00000000 @ 0x00000000.
Transferring frame 0x00000001 @ 0x000007D8.
Transferring frame 0x00000002 @ 0x00000FB0.
Transferring frame 0x00000003 @ 0x00001788.
Transferring frame 0x00000004 @ 0x00001F60.
Transferring frame 0x00000005 @ 0x00002738.
Transferring frame 0x00000006 @ 0x00002F10.
Transferring frame 0x80000007 @ 0x000036E8.
Loading done.
```

And then on the serial port something like this:


```
I00001 bootstrap.c:153] Bootstrap requested, initialising HW...
I00002 bootstrap.c:177] HW initialisation completed, waiting for SPI input...
I00003 bootstrap.c:101] Processing frame #0, expecting #0
0004 bootstrap.c:124] Flash erase successful
I00005 bootstrap.c:101] Processing frame #1, expecting #1
I00006 bootstrap.c:101] Processing frame #2, expecting #2
I00007 bootstrap.c:101] Processing frame #3, expecting #3
I00008 bootstrap.c:101] Processing frame #4, expecting #4
I00009 bootstrap.c:101] Processing frame #5, expecting #5
I00010 bootstrap.c:101] Processing frame #6, expecting #6
I00011 bootstrap.c:101] Processing frame #7, expecting #7
I00012 bootstrap.c:134] Bootstrap: DONE!
I00013 boot_rom.c:62] Boot ROM initialisation has completed, jump into flash!
I00000 hello_world.c:65] Hello World!
I00001 hello_world.c:66] Built at: Apr  2 2021, 20:15:30
I00002 demos.c:18] Watch the LEDs!
I00003 hello_world.c:73] Try out the switches on the board
I00004 hello_world.c:74] or type anything into the console window.
I00005 hello_world.c:75] The LEDs show the ASCII code of the last character.
I00006 demos.c:50] GPIO switch #0 changed to 1
I00007 demos.c:50] GPIO switch #1 changed to 1
I00008 demos.c:50] GPIO switch #2 changed to 1
I00009 demos.c:50] GPIO switch #3 changed to 1
I00010 demos.c:56] FTDI control changed. Enable JTAG.
```

### Testing USB Phy

The test server has a PhyWhisperer plugged inbetween the OpenTitan USB port & host server. This allows you to virtually unplug & replug the USB connection, without needing to do any "hacks" on the OpenTitan bitstream to fake the connection/disconnect event. To set this up, first install the phywhispererusb control library (we'll install from git to ensure we have the latest version, you can normally use pip as well):

```
git clone https://github.com/newaetech/phywhispererusb.git
cd phywhispererusb
python3 setup.py install --user
```

Put the following code in a file called `phywhisperer-toggle.py`

```
#!/usr/bin/env python3
import time
import phywhisperer.usb as pw
phy = pw.Usb()
phy.con()
phy.set_power_source("off")
time.sleep(1)
phy.set_power_source("host")
```

And set it to be executable:
```console
chmod a+x phywhisperer-toggle.py
```

Next, load the `hello_usbdev` image:
```
cd $REPO_TOP
./util/bergenloader.py -fw temp_build_cache/hello_usbdev_fpga_nexysvideo.bin
```

If you haven't loaded the FPGA yet, you could load them both at once with:

```
./util/bergenloader.py -fw temp_build_cache/hello_usbdev_fpga_nexysvideo.bin -bs temp_build_cache/lowrisc_systems_top_earlgrey_bergen-k410t_0.1.bit
```

Now run the power cycle script to unplug/replug the opentitan to trigger the enumeration:

```
./phywhisperer-toggle.py
```

And check the results with `dmesg`, hopefully you see something like this:

```
[14229.727907] usb 1-3.1: new full-speed USB device number 30 using ehci-pci
[14229.838082] usb 1-3.1: New USB device found, idVendor=18d1, idProduct=503a
[14229.838086] usb 1-3.1: New USB device strings: Mfr=0, Product=0, SerialNumber=0
[14229.838512] usb_serial_simple 1-3.1:1.0: google converter detected
[14229.838695] usb 1-3.1: google converter now attached to ttyUSB0
[14229.838768] usb_serial_simple 1-3.1:1.1: google converter detected
[14229.838935] usb 1-3.1: google converter now attached to ttyUSB1
```
If it doesn't work right away, try the `phywhisperer-toggle` script again. If this works, you can then access the attached serial ports, note you don't need a baud rate as the data is always processed internally in the OT chip.

```console
screen /dev/ttyUSB0
```

Characters you type on this should appear on the serial port you started with `bergen-serial.sh` on `/dev/ttyACM1`.

#### Sniffing USB traffic with PhyWhisperer-USB

The PhyWhisperer-USB also allows you to sniff some of the data. Note that PhyWhisperer-USB is designed as a triggering hardware and not as a sniffer, so sniffing functionality is limited to a small 8191 entry buffer (but enough for enumeration sniffing in this example).

To test the sniffing, copy the following code to a file which you save as `phywhisperer-sniff-enum.py` & again make executable.

```python
#!/usr/bin/env python3

import time
import phywhisperer.usb as pw
phy = pw.Usb()
phy.con()
phy.addpattern = True #Adds captured data back, otherwise we capture 'after'
phy.set_power_source("off")
phy.reset_fpga()
phy.set_pattern([0x2d, 0x00])
time.sleep(1)
phy.arm()
phy.set_power_source("host")
time.sleep(2)
#wait a second for device enumeration, 'capturing' LED goes out
raw = phy.read_capture_data()
packets = phy.split_packets(raw)
printPackets = pw.USBSimplePrintSink(highspeed=phy.get_usb_mode() == 'HS')
for packet in packets:
    printPackets.handle_usb_packet(ts=packet['timestamp'], buf=bytearray(packet['contents']), flags=0)
```

Then run this file - this will disconnect & reconnect the USB port, while also looking for some enumeration traffic. Note that the PhyWhisperer waits until it sees the USB sequence `0x2d`, `0x00` to start capturing, so no data is returned if the link doesn't come up at all.

```console
./phywhisperer-sniff-enum.py
WARNING:root:FIFO overflowed, capture stopped.
WARNING:root:8191 entries captured.
[      ]   0.000000 d=  0.000000 [   .0 +  0.017] [  3] SETUP: 0.0
[      ]   0.000001 d=  0.000001 [   .0 +  1.433] [ 11] DATA0: 80 06 00 01 00 00 40 00 dd 94
[      ]   0.000010 d=  0.000008 [   .0 +  9.933] [  1] ACK
[      ]   0.000017 d=  0.000008 [   .0 + 17.433] [  3] IN   : 0.0
[      ]   0.000021 d=  0.000003 [   .0 + 20.600] [  1] NAK
[      ]   0.000025 d=  0.000005 [   .0 + 25.433] [  3] IN   : 0.0
[      ]   0.000029 d=  0.000003 [   .0 + 28.683] [  1] NAK
[      ]   0.000034 d=  0.000005 [   .0 + 33.517] [  3] IN   : 0.0
[      ]   0.000037 d=  0.000003 [   .0 + 36.683] [  1] NAK
[      ]   0.000043 d=  0.000007 [   .0 + 43.267] [  3] IN   : 0.0
[      ]   0.000046 d=  0.000003 [   .0 + 46.433] [  1] NAK
[      ]   0.000054 d=  0.000007 [   .0 + 53.850] [  3] IN   : 0.0
[      ]   0.000057 d=  0.000003 [   .0 + 57.017] [  1] NAK
[      ]   0.000064 d=  0.000007 [   .0 + 64.267] [  3] IN   : 0.0
[      ]   0.000068 d=  0.000003 [   .0 + 67.517] [  1] NAK
[      ]   0.000072 d=  0.000005 [   .0 + 72.350] [  3] IN   : 0.0
[      ]   0.000076 d=  0.000003 [   .0 + 75.600] [  1] NAK
[      ]   0.000080 d=  0.000005 [   .0 + 80.433] [  3] IN   : 0.0
[      ]   0.000084 d=  0.000003 [   .0 + 83.600] [  1] NAK
[      ]   0.000088 d=  0.000005 [   .0 + 88.433] [  3] IN   : 0.0
[      ]   0.000092 d=  0.000003 [   .0 + 91.683] [  1] NAK
[      ]   0.000097 d=  0.000005 [   .0 + 96.517] [  3] IN   : 0.0
[      ]   0.000100 d=  0.000003 [   .0 + 99.767] [  1] NAK
[      ]   0.000105 d=  0.000005 [   .0 +104.600] [  3] IN   : 0.0
[      ]   0.000108 d=  0.000003 [   .0 +107.767] [  1] NAK
[      ]   0.000113 d=  0.000005 [   .0 +112.683] [  3] IN   : 0.0
[      ]   0.000116 d=  0.000003 [   .0 +115.850] [ 21] DATA1: 12 01 00 02 00 00 00 40 d1 18 3a 50 00 01 00 00 00 01 7d 34
[      ]   0.000131 d=  0.000015 [   .0 +131.267] [  1] ACK
[      ]   0.000136 d=  0.000004 [   .0 +135.600] [  3] OUT  : 0.0
[      ]   0.000139 d=  0.000003 [   .0 +139.017] [  3] DATA1: 00 00
[      ]   0.000142 d=  0.000003 [   .0 +142.183] [  1] ACK
[      ]   0.079992 d=  0.079850 [ 49   +451.333] [  3] SETUP: 0.0
[      ]   0.079995 d=  0.000003 [ 49   +454.750] [ 11] DATA0: 00 05 1b 00 00 00 00 00 e9 1f
[      ]   0.080004 d=  0.000009 [ 49   +463.333] [  1] ACK
[      ]   0.080010 d=  0.000006 [ 49   +469.250] [  3] IN   : 0.0
[      ]   0.080013 d=  0.000003 [ 49   +472.417] [  1] NAK
[      ]   0.080018 d=  0.000005 [ 49   +477.250] [  3] IN   : 0.0
[      ]   0.080021 d=  0.000003 [ 49   +480.500] [  1] NAK
[      ]   0.080026 d=  0.000005 [ 49   +485.333] [  3] IN   : 0.0
[      ]   0.080029 d=  0.000003 [ 49   +488.500] [  1] NAK
[      ]   0.080034 d=  0.000005 [ 49   +493.417] [  3] IN   : 0.0
[      ]   0.080037 d=  0.000003 [ 49   +496.583] [  1] NAK
[      ]   0.080045 d=  0.000007 [ 49   +503.917] [  3] IN   : 0.0
[      ]   0.080048 d=  0.000003 [ 49   +507.083] [  1] NAK
[      ]   0.080055 d=  0.000007 [ 49   +514.500] [  3] IN   : 0.0
[      ]   0.080058 d=  0.000003 [ 49   +517.667] [  1] NAK
[      ]   0.080065 d=  0.000007 [ 49   +524.833] [  3] IN   : 0.0
[      ]   0.080069 d=  0.000003 [ 49   +528.083] [  1] NAK
[      ]   0.080074 d=  0.000005 [ 49   +532.917] [  3] IN   : 0.0
[      ]   0.080077 d=  0.000003 [ 49   +536.167] [  1] NAK
[      ]   0.080082 d=  0.000005 [ 49   +541.083] [  3] IN   : 0.0
[      ]   0.080085 d=  0.000003 [ 49   +544.250] [  1] NAK
[      ]   0.080090 d=  0.000005 [ 49   +549.083] [  3] IN   : 0.0
[      ]   0.080093 d=  0.000003 [ 49   +552.333] [  1] NAK
[      ]   0.080098 d=  0.000005 [ 49   +557.250] [  3] IN   : 0.0
[      ]   0.080101 d=  0.000003 [ 49   +560.500] [  1] NAK
[      ]   0.080106 d=  0.000005 [ 49   +565.417] [  3] IN   : 0.0
[      ]   0.080109 d=  0.000003 [ 49   +568.583] [  3] DATA1: 00 00
[      ]   0.080113 d=  0.000003 [ 49   +572.083] [  1] ACK
[      ]   0.107992 d=  0.027880 [ 77   +451.250] [  3] SETUP: 27.0
[      ]   0.107996 d=  0.000003 [ 77   +454.667] [ 11] DATA0: 80 06 00 01 00 00 12 00 e0 f4
...many more lines...
```

This shows you sniffing the USB traffic & confirming the OT USB phy is working.

### Screen error 

Often when you run the serial command with `screen` you will just get

```
[screen is terminating]
```

This happens when the serial port is already in use or you don't have permissions as required.

Likely someone else is actively using it, or their ssh connection died while screen was running (which may not kill screen automatically - we tried to disable autodisconnect as an option anyway). To fix this use `sudo killall screen` to kill all sessions if you have sudo access. 
