---
title: "Getting started with a Bergen Board"
---

Do you want to try out the lowRISC chip designs and have a few grand for a nice Bergen Board? While you are in the right place!

<!-- TODO: Switch all calls to fusesoc and the Verilated system to refer to Meson, once it supports fusesoc. -->

See the page [Getting Started on FPGAs]({{< relref "getting_started_fpga.md" >}}) for a more general guide. This page is designed to bring you up to speed on OpenTitan,
when specifically targeting the ChipWhisperer CW310 "Bergen Board". It assumes limited previous experience with OpenTitan and general knowledge of FPGA development.

If you already have used OpenTitan, see the page [Bergen Board for OpenTitan Developers]({{< relref "bergen_developer_quickstart.md" >}}) which skips some of the
setup information. You might want to read over this page for a background on how to plug in the Bergen Board still.

## About FPGA Bitstream & Software (Firmware) images

You'll need to do two steps to work with OpenTitan on a FPGA board:

1. Build the bitstream which is the base system, and integrates the "boot ROM".
2. Build your user software that runs on the base system.

The first step is the longest & most complicated, but required if you plan on modifying any of the RTL code. If you just want to work on the software side, which interacts with the existing code, you can use pre-built FPGA bitstreams. If you are just getting started you might want to use a pre-built bitstream to confirm your board is working ok, and then try to rebuild the bitstream for this board.

**WARNING**: The FPGA bitstream & software build need to match, which is more likely to break during active development. For basic testing of the board a pre-built FPGA image and pre-built bitstream are both provided. We call this the "Hello World Demo" and gets you up to speed without doing *any* build operations (yay!).

Once you see this working, you can rebuild the FPGA bitstream and software. You now have a totally verifiable open-source system which you can modify.

## Hello World Demo

We'll first connect the board to your system, and then see how to program it. Before we do this, you should do some minor computer setup (if not done already).

Either edit or create a file `90-opentitan.rules` file in `/etc/udev/rules.d` which will also allow you access to the Bergen Board (along with other devices in the same product line):

```
# ChipWhisperer Bergen Board
ACTION=="add|change", SUBSYSTEM=="usb|tty", ATTRS{idVendor}=="2b3e", ATTRS{idProduct}=="ace[0-9]|c[3-6][0-9][0-9]", MODE="0666"
```

And reload the udev rules with:

```console
sudo udevadm control --reload-rules`
```

Talking to the Bergen Board uses the ChipWhisperer python code - for now you should track the git version with:

```console
cd $somewhere
git clone https://github.com/newaetech/chipwhisperer
cd chipwhisperer
python3 setup.py develop --user
```

**NOTE:** You may get an error during the install process as it tries to compile `numpy` and `scipy` from sources. If this happens instead use your package manager
to install them with:

```console
sudo apt-get install python-numpy python-scipy
```

Then try running the `setup.py` file again. You may also need to `pip3 install setup_tools` if using a stock distribution which doesn't have `setup_tools`.

### Physical Connections

Place the Bergen Board in front of you and enjoy it in all it's glory. For more information on the Bergen Board see XXXX, this guide covers only what is required for
OpenTitan usage.

![Bergen Board image](../bergen_board/img/bergenk410t_annotated_ot.jpg)

You'll have to adjust some switches based on your power choices. You can choose to:

1. Power the board via USB only.
2. Power the board via a barrel jack / wall wart.
3. Power the board via a USB-C charger.

The first choice is the most convient, but requires you to plug the board into a USB-C host port or through a powered USB hub. In addition some designs will take more power than is available from your USB computer port. If this happens you'll get "USB surge" error messages and could even damage your host computer.

The easiest option is often using the included 'wall wart' transformer. This provides 12V for the board. To use this option:

1. Set switch SW2 (marked at ①) up to the `5V Regulator` option.
2. Set switch SW (marked at ②) to the right towards the `Barrel` option.
4. Plug the DC power adapter into the barrel jack at ②.
5. Plug your USB-C cable from your host computer (via USB-A to USB-C adapter if required) into USB-C port near ③.
6. Turn the `Control Power` switch ③ to the right to turn on the controller.

If you insist on living dangerously, you can instead power everything from the single USB-C port with:

1. Set switch SW2 (marked at ①) down to the `USB-C Data` option.
2. Plug your USB-C cable from your host computer into USB-C port near ③.
3. Turn the `Control Power` switch ③ to the right to turn on the controller.

The `Status` led D23 should be blinking blue once the board is connected. If it appears as just solid blue try toggling the `Control Power` switch, or press the `USB RST` button. You may need to check your `udev` settings or wait for driver configuration.

### Serial Connection

The Bergen Board has two serial ports (just like FTDI serial adapters) that are routed to the FPGA. These allow you to view debug and other messages, or otherwise interact with the system.

You can check what they appear as with `dmesg` - on the default Ubuntu configuration they appear as `\dev\ttyACM0` and `\dev\ttyACM1`. The second port is connected to the standard OpenTitan output (`\dev\ttyACM1` in these examples).

You can use a serial program such as `minicom` or `screen` to connect at 115200 baud - be sure to disable software and hardware flow control if enabled by default. With screen for example we would use:

```console
$ screen /dev/ttyACM1 115200,cs8,-ixon,-ixoff
```

A lazy script is provided at `./util/bergen-serial.sh` which runs this command. Other options (such as `8n1` instead of `cs8`) don't seem to work, so when setting up please use this command exactly as-is.

**NOTE:** If you haven't used screen before - exit screen with `Ctrl-A` then `k`, and confirm with `y`. Note if you "detach" the screen terminal (for example if running screen via ssh and close the ssh connection) the serial port will stay open and in use by screen.

### Loading the FPGA & Firmware

Assuming you have the `screen` connection, you can then run the following in a second console:

```console
$ cd $REPO_TOP
$ ./util/bergenloader.py -bs temp_build_cache/lowrisc_systems_top_earlgrey_bergen-k410t_0.1.bit -fw temp_build_cache/hello_world_fpga_nexysvideo.bin
```

This loads both the FPGA bitstream and the software onto the FPGA design. The output should look like this:

```
Bergen Loader: Attemping to find CW310 Bergen Board:
    Using bitstream :temp_build_cache/lowrisc_systems_top_earlgrey_bergen-k410t_0.1.bit
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

If you just want to load new software, you can use the command `./util/bergenloader.py -fw <binary_file>`. Give it a shot by reloading the software running:

```console
$ cd $REPO_TOP
$ ./util/bergenloader.py -fw temp_build_cache/hello_world_fpga_nexysvideo.bin
```

Looking back on the serial port, you should see the following messages if everything is working:

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

If so - you did it! You got OpenTitan running on the FPGA board. You can change the state of the first four user DIP switches, and if you type characters the first three LEDs
will change state.

**NOTE:** This can be fixed to use all LEDs, issue in current earlygrey top.

Once done - read on to do a full build.

## Prerequisites

To use the lowRISC designs on an FPGA you need two things:

* A CW310 Bergen Board with a Xilinx Kintex K410T FPGA mounted.
* A tool from the FPGA vendor (Vivado).

Follow the install instructions to [prepare the system]({{< relref "install_instructions#system-preparation" >}}) and to install the [software development tools]({{< relref "install_instructions#software-development" >}}) and [Xilinx Vivado]({{< relref "install_instructions#xilinx-vivado" >}}).

In addition, you'll need a *Vivado Design Suite* license - you can request a "30-Day Evaluation License" via the online license management tool, this will be locked to your node (via Ethernet mac address or similar). You may be able to request additional trial licenses. Most universities have an ability to acquire these licenses via the "University Program" as well.

If you don't set this license up, you'll get an error during the `fusesoc` process.

## Create an FPGA bitstream

Synthesizing a design for a FPGA board is done with the following commands.

The FPGA build will pull in a program to act as the boot ROM.
This must be built before running the FPGA build.
This is pulled in from the `sw/device/boot_rom` directory (see the `parameters:` section of the `hw/top_earlgrey/top_earlgrey_bergen-k410t.core` file).

To build it:
```console
$ cd $REPO_TOP
$ ./meson_init.sh
$ ninja -C build-out all
```

In the following example we synthesize the Earl Grey design for the Bergen Video board using Xilinx Vivado 2020.1.

```console
$ . /tools/xilinx/Vivado/2020.1/settings64.sh
$ cd $REPO_TOP
$ ./meson_init.sh
$ ninja -C build-out all
$ fusesoc --cores-root . run --flag=fileset_top --target=synth lowrisc:systems:top_earlgrey_bergen-k410t
```
The `fileset_top` flag used above is specific to the OpenTitan project to select the correct fileset.

The resulting bitstream is located at `build/lowrisc_systems_top_earlgrey_nexysvideo_0.1/synth-vivado/lowrisc_systems_top_earlgrey_bergen-k410t_0.1.bit`.
See the [reference manual]({{< relref "ref_manual_fpga.md" >}}) for more information.


## Testing the demo design

The `hello_world` demo software shows off some capabilities of the design.
In order to load `hello_world` into the FPGA the binary must be compiled.
Please follow the steps shown below.

First, you'll need to flash the output bitstream generated by the `fusesoc` run. The default location of it would mean using the following command:

```console
$ ./util/bergenloader.py -bs build/lowrisc_systems_top_earlgrey_bergen-k410t_0.1/synth-vivado/lowrisc_systems_top_earlgrey_bergen-k410t_0.1.bit
```

Then open a serial console (see earlier for more details - we'll use the helper script):

```console
$ ./util/bergen-serial.sh
```

Your earlier command would have built the `helloworld` binary, but you can rebuild just the one program with:

```console
$ cd ${REPO_TOP}
$ ninja -C build-out sw/device/examples/hello_world/hello_world_export_fpga_nexysvideo
```

Finally, program in the software with:

```console
./util/bergenloader.py -fw build-bin/sw/device/examples/hello_world/hello_world_fpga_nexysvideo.bin
```

Observe the output both on the board and the serial console. Type any text into the console window.

Exit `screen` by pressing CTRL-a k, and confirm with y.

## Develop with the Vivado GUI

Sometimes it is helpful to use the Vivado GUI to debug a design.
fusesoc makes that easy, with one small caveat: by default fusesoc copies all source files into a staging directory before the synthesis process starts.
This behavior is helpful to create reproducible builds and avoids Vivado modifying checked-in source files.
But during debugging this behavior is not helpful.
The `--no-export` option of fusesoc disables copying the source files into the staging area, and `--setup` instructs fusesoc to only create a project file, but not to run the synthesis process.

```console
$ # only create Vivado project file
$ fusesoc --cores-root . build --no-export --setup lowrisc:systems:top_earlgrey_nexysvideo
```

