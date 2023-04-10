#!/usr/bin/env python3
# Copyright lowRISC contributors.
# Licensed under the Apache License, Version 2.0, see LICENSE for details.
# SPDX-License-Identifier: Apache-2.0

import argparse

import chipwhisperer as cw
from chipwhisperer.capture.targets.CW340 import CW340


def main():

    parser = argparse.ArgumentParser(
        description=
        'Load OpenTitan Bitstream & Software to ChipWhisperer CW310 FPGA Board'
    )

    parser.add_argument('--bitstream',
                        '-b',
                        metavar='bitstream',
                        type=str,
                        help='Path to the FPGA .bit file to load',
                        default=None)

    parser.add_argument('--set-pll-defaults',
                        action='store_true',
                        help='Program on-board PLL with defaults',
                        default=False)

    args = parser.parse_args()

    print("CW310 Loader: Attemping to find CW310 FPGA Board:")
    if args.bitstream:
        print("    Using bitstream :{}".format(args.bitstream))
    else:
        print("    No bitstream specified")
    target = cw.target(None, CW340, bsfile=args.bitstream, slurp=False, prog_mode="parallel")

    print("CW310 Board Found:")

    if args.set_pll_defaults:
        print("Configuring PLL, setting as default")
        target.pll.pll_enable_set(True)
        target.pll.pll_outenable_set(False, 0)
        # Note: 1 and 2 seem to be reversed.
        target.pll.pll_outenable_set(True, 1)
        target.pll.pll_outenable_set(False, 2)

        # Note: both 1 and 2 need to be set, even if only 1 is enabled.
        target.pll.pll_outfreq_set(100E6, 1)
        target.pll.pll_outfreq_set(100E6, 2)
        
        target.pll.pll_writedefaults()


    print("Loading done.")


if __name__ == "__main__":
    main()
