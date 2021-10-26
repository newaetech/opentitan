
## Opening serial port

screen /dev/ttyACM1 115200,cs8,-ixon,-ixoff


"Ctrl-A" then "\" to kill screen.

## Firmware program

Pre-built BS ('should' work with SW built on same system)
```
util/fpga/./cw310_loader.py -b temp-test-scaecc/lowrisc_systems_chip_earlgrey_cw310_0.1.bit
```

Building own FPGA BS
```
util/fpga/./cw310_loader.py -b build/lowrisc_systems_chip_earlgrey_cw310_0.1/synth-vivado/lowrisc_systems_chip_earlgrey_cw310_0.1.bit
```

NOTE - does not overwrite FPGA by default.

## Build & Program SW into device:

```
ninja -C build-out all
util/fpga/./cw310_loader.py --firmware build-bin/sw/device/tests/otbn_ecdsa_p256_test_fpga_cw310.bin
```
