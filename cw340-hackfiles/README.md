
build the FPGA, then:

```
$cd $OPENTITAN
$ python3 util/fpga/cw340_loader.py --bitstream bazel-bin/hw/bitstream/vivado/build.fpga_cw310/synth-vivado/lowrisc_systems_chip_earlgrey_cw310_0.1.bit
```

NOTE: A copy of `lowrisc_systems_chip_earlgrey_cw310_0.1.bit` exists in cw340-hackfiles in case you want to just use the pre-built version



screen /dev/ttyUSB2 115200,cs8,-ixon,-ixoff

```
bazel run //sw/host/opentitantool -- --interface=cw310 fpga set-pll
```

## Loading with SAM3X

* Jumper J4-Pin3 to JP5-3. This will enable a nRST to be sent on current bitstream.
* Set JP11/JP2 to "SAM" to enable strapping pin control (not sure if this is needed?)


```
bazel run //sw/host/opentitantool bootstrap $(ci/scripts/target-location.sh //sw/device/examples/hello_usbdev:hello_usbdev_fpga_cw310_bin)
```

```
bazel run //sw/host/opentitantool bootstrap $(ci/scripts/target-location.sh //sw/device/examples/hello_world:hello_world_fpga_cw310_bin)
```

## Loading with JTAG
* Set JP11/JP12 to "FTDI" to enable strapping pin control via FTDI

openocd -f util/openocd/board/cw340.cfg -f util/openocd/target/lowrisc-earlgrey.cfg

