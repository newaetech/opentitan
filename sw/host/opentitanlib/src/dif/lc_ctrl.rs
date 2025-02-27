// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

use crate::with_unknown;
use num_enum::IntoPrimitive;

with_unknown! {
    pub enum DifLcCtrlState: u32 {
        Raw = bindgen::dif::dif_lc_ctrl_state_kDifLcCtrlStateRaw as u32,
        TestUnlocked0 = bindgen::dif::dif_lc_ctrl_state_kDifLcCtrlStateTestUnlocked0 as u32,
        TestLocked0 = bindgen::dif::dif_lc_ctrl_state_kDifLcCtrlStateTestLocked0 as u32,
        TestUnlocked1 = bindgen::dif::dif_lc_ctrl_state_kDifLcCtrlStateTestUnlocked1 as u32,
        TestLocked1 = bindgen::dif::dif_lc_ctrl_state_kDifLcCtrlStateTestLocked1 as u32,
        TestUnlocked2 = bindgen::dif::dif_lc_ctrl_state_kDifLcCtrlStateTestUnlocked2 as u32,
        TestLocked2 = bindgen::dif::dif_lc_ctrl_state_kDifLcCtrlStateTestLocked2 as u32,
        TestUnlocked3 = bindgen::dif::dif_lc_ctrl_state_kDifLcCtrlStateTestUnlocked3 as u32,
        TestLocked3 = bindgen::dif::dif_lc_ctrl_state_kDifLcCtrlStateTestLocked3 as u32,
        TestUnlocked4 = bindgen::dif::dif_lc_ctrl_state_kDifLcCtrlStateTestUnlocked4 as u32,
        TestLocked4 = bindgen::dif::dif_lc_ctrl_state_kDifLcCtrlStateTestLocked4 as u32,
        TestUnlocked5 = bindgen::dif::dif_lc_ctrl_state_kDifLcCtrlStateTestUnlocked5 as u32,
        TestLocked5 = bindgen::dif::dif_lc_ctrl_state_kDifLcCtrlStateTestLocked5 as u32,
        TestUnlocked6 = bindgen::dif::dif_lc_ctrl_state_kDifLcCtrlStateTestUnlocked6 as u32,
        TestLocked6 = bindgen::dif::dif_lc_ctrl_state_kDifLcCtrlStateTestLocked6 as u32,
        TestUnlocked7 = bindgen::dif::dif_lc_ctrl_state_kDifLcCtrlStateTestUnlocked7 as u32,
        Dev = bindgen::dif::dif_lc_ctrl_state_kDifLcCtrlStateDev as u32,
        Prod = bindgen::dif::dif_lc_ctrl_state_kDifLcCtrlStateProd as u32,
        ProdEnd = bindgen::dif::dif_lc_ctrl_state_kDifLcCtrlStateProdEnd as u32,
        Rma = bindgen::dif::dif_lc_ctrl_state_kDifLcCtrlStateRma as u32,
        Scrap = bindgen::dif::dif_lc_ctrl_state_kDifLcCtrlStateScrap as u32,
        PostTransition = bindgen::dif::dif_lc_ctrl_state_kDifLcCtrlStatePostTransition as u32,
        Escalate = bindgen::dif::dif_lc_ctrl_state_kDifLcCtrlStateEscalate as u32,
        StateInvalid = bindgen::dif::dif_lc_ctrl_state_kDifLcCtrlStateInvalid as u32,
    }
}

impl DifLcCtrlState {
    /// Encode the given life cycle state in a redundant format where the
    /// five-bit value is repeated six times.
    pub fn redundant_encoding(&self) -> u32 {
        let value: u32 = self.0;
        assert_eq!(value & 0b11111, value);
        (0..6).fold(0u32, |acc, _| (acc << 5) | value)
    }
}

pub struct DifLcCtrlToken(bindgen::dif::dif_lc_ctrl_token);

impl From<[u8; 16]> for DifLcCtrlToken {
    fn from(bytes: [u8; 16]) -> Self {
        DifLcCtrlToken(bindgen::dif::dif_lc_ctrl_token { data: bytes })
    }
}

impl DifLcCtrlToken {
    /// Converts a 128-bit transition token into four native u32 words. These
    /// values are suitable to write to [LcCtrlReg::TransitionToken0] and
    /// friends.
    pub fn into_register_values(self) -> [u32; 4] {
        let mut out_words = [0u32; 4];
        let bytes = self.0.data;
        bytes
            .chunks_exact(std::mem::size_of::<u32>())
            .map(|chunk| u32::from_le_bytes(chunk.try_into().unwrap()))
            .zip(&mut out_words)
            .for_each(|(word, out)| *out = word);
        out_words
    }
}

#[derive(IntoPrimitive, Clone, Debug)]
#[repr(u32)]
pub enum LcCtrlReg {
    AlertTest = bindgen::dif::LC_CTRL_ALERT_TEST_REG_OFFSET,
    Status = bindgen::dif::LC_CTRL_STATUS_REG_OFFSET,
    ClaimTransitionIf = bindgen::dif::LC_CTRL_CLAIM_TRANSITION_IF_REG_OFFSET,
    TransitionRegwen = bindgen::dif::LC_CTRL_TRANSITION_REGWEN_REG_OFFSET,
    TransitionCmd = bindgen::dif::LC_CTRL_TRANSITION_CMD_REG_OFFSET,
    TransitionCtrl = bindgen::dif::LC_CTRL_TRANSITION_CTRL_REG_OFFSET,
    TransitionToken0 = bindgen::dif::LC_CTRL_TRANSITION_TOKEN_0_REG_OFFSET,
    TransitionToken1 = bindgen::dif::LC_CTRL_TRANSITION_TOKEN_1_REG_OFFSET,
    TransitionToken2 = bindgen::dif::LC_CTRL_TRANSITION_TOKEN_2_REG_OFFSET,
    TransitionToken3 = bindgen::dif::LC_CTRL_TRANSITION_TOKEN_3_REG_OFFSET,
    TransitionTarget = bindgen::dif::LC_CTRL_TRANSITION_TARGET_REG_OFFSET,
    OtpVendorTestCtrl = bindgen::dif::LC_CTRL_OTP_VENDOR_TEST_CTRL_REG_OFFSET,
    OtpVendorTestStatus = bindgen::dif::LC_CTRL_OTP_VENDOR_TEST_STATUS_REG_OFFSET,
    LcState = bindgen::dif::LC_CTRL_LC_STATE_REG_OFFSET,
    LcTransitionCnt = bindgen::dif::LC_CTRL_LC_TRANSITION_CNT_REG_OFFSET,
    LcIdState = bindgen::dif::LC_CTRL_LC_ID_STATE_REG_OFFSET,
    HwRev = bindgen::dif::LC_CTRL_HW_REV_REG_OFFSET,
    DeviceId0 = bindgen::dif::LC_CTRL_DEVICE_ID_0_REG_OFFSET,
    DeviceId1 = bindgen::dif::LC_CTRL_DEVICE_ID_1_REG_OFFSET,
    DeviceId2 = bindgen::dif::LC_CTRL_DEVICE_ID_2_REG_OFFSET,
    DeviceId3 = bindgen::dif::LC_CTRL_DEVICE_ID_3_REG_OFFSET,
    DeviceId4 = bindgen::dif::LC_CTRL_DEVICE_ID_4_REG_OFFSET,
    DeviceId5 = bindgen::dif::LC_CTRL_DEVICE_ID_5_REG_OFFSET,
    DeviceId6 = bindgen::dif::LC_CTRL_DEVICE_ID_6_REG_OFFSET,
    DeviceId7 = bindgen::dif::LC_CTRL_DEVICE_ID_7_REG_OFFSET,
    ManufState0 = bindgen::dif::LC_CTRL_MANUF_STATE_0_REG_OFFSET,
    ManufState1 = bindgen::dif::LC_CTRL_MANUF_STATE_1_REG_OFFSET,
    ManufState2 = bindgen::dif::LC_CTRL_MANUF_STATE_2_REG_OFFSET,
    ManufState3 = bindgen::dif::LC_CTRL_MANUF_STATE_3_REG_OFFSET,
    ManufState4 = bindgen::dif::LC_CTRL_MANUF_STATE_4_REG_OFFSET,
    ManufState5 = bindgen::dif::LC_CTRL_MANUF_STATE_5_REG_OFFSET,
    ManufState6 = bindgen::dif::LC_CTRL_MANUF_STATE_6_REG_OFFSET,
    ManufState7 = bindgen::dif::LC_CTRL_MANUF_STATE_7_REG_OFFSET,
}

impl LcCtrlReg {
    pub fn byte_offset(&self) -> u32 {
        self.clone().into()
    }
    /// Converts the register's byte offset into a word offset for use with DMI.
    /// https://docs.opentitan.org/hw/ip/lc_ctrl/doc/#life-cycle-tap-controller
    pub fn word_offset(&self) -> u32 {
        const BYTES_PER_WORD: u32 = std::mem::size_of::<u32>() as u32;
        assert_eq!(self.byte_offset() % BYTES_PER_WORD, 0);
        self.byte_offset() / BYTES_PER_WORD
    }
}

pub trait LcBit: Clone + Into<u32> {
    /// Builds a register value from a collection of [LcBit] bits.
    fn union<const N: usize>(bits: [Self; N]) -> u32 {
        bits.into_iter()
            .map(|bit| bit.into())
            .map(|bit: u32| 1 << bit)
            .fold(0u32, |acc, shifted| acc | shifted)
    }
}

/// Bits of the lc_ctrl.STATUS register, aka [LcCtrlReg::Status].
#[derive(IntoPrimitive, Clone, Debug)]
#[repr(u32)]
pub enum LcCtrlStatusBit {
    Initialized = bindgen::dif::LC_CTRL_STATUS_INITIALIZED_BIT,
    Ready = bindgen::dif::LC_CTRL_STATUS_READY_BIT,
    TransitionSuccessful = bindgen::dif::LC_CTRL_STATUS_TRANSITION_SUCCESSFUL_BIT,
    TransitionCountError = bindgen::dif::LC_CTRL_STATUS_TRANSITION_COUNT_ERROR_BIT,
    TransitionError = bindgen::dif::LC_CTRL_STATUS_TRANSITION_ERROR_BIT,
    TokenError = bindgen::dif::LC_CTRL_STATUS_TOKEN_ERROR_BIT,
    FlashRmaError = bindgen::dif::LC_CTRL_STATUS_FLASH_RMA_ERROR_BIT,
    OtpError = bindgen::dif::LC_CTRL_STATUS_OTP_ERROR_BIT,
    StateError = bindgen::dif::LC_CTRL_STATUS_STATE_ERROR_BIT,
    BusIntegError = bindgen::dif::LC_CTRL_STATUS_BUS_INTEG_ERROR_BIT,
    OtpPartitionError = bindgen::dif::LC_CTRL_STATUS_OTP_PARTITION_ERROR_BIT,
}
impl LcBit for LcCtrlStatusBit {}

/// Bits of the lc_ctrl.TRANSITION_REGWEN register, aka [LcCtrlReg::TransitionRegwen].
#[derive(IntoPrimitive, Clone, Debug)]
#[repr(u32)]
pub enum LcCtrlTransitionRegwenBit {
    TransitionRegwen = bindgen::dif::LC_CTRL_TRANSITION_REGWEN_TRANSITION_REGWEN_BIT,
}
impl LcBit for LcCtrlTransitionRegwenBit {}

/// Bits of the lc_ctrl.TRANSITION_CMD register, aka [LcCtrlReg::TransitionCmd].
#[derive(IntoPrimitive, Clone, Debug)]
#[repr(u32)]
pub enum LcCtrlTransitionCmdBit {
    Start = bindgen::dif::LC_CTRL_TRANSITION_CMD_START_BIT,
}
impl LcBit for LcCtrlTransitionCmdBit {}

/// Bits of the lc_ctrl.TRANSITION_CTRL register, aka [LcCtrlReg::TransitionCtrl].
#[derive(IntoPrimitive, Clone, Debug)]
#[repr(u32)]
pub enum LcCtrlTransitionCtrlBit {
    ExtClockEn = bindgen::dif::LC_CTRL_TRANSITION_CTRL_EXT_CLOCK_EN_BIT,
}
impl LcBit for LcCtrlTransitionCtrlBit {}

#[cfg(test)]
mod tests {
    use super::*;

    /// Raw is zero, so its redundant encoding is a fixed point.
    #[test]
    fn lc_ctrl_state_redundant_encoding_zero() {
        assert_eq!(u32::from(DifLcCtrlState::Raw), 0);
        assert_eq!(DifLcCtrlState::Raw.redundant_encoding(), 0);
    }

    /// The redundant encoding of non-zero values shouldn't be a fixed point.
    #[test]
    fn lc_ctrl_state_redundant_encoding_nonzero() {
        assert_ne!(
            u32::from(DifLcCtrlState::Rma),
            DifLcCtrlState::Rma.redundant_encoding()
        );
        assert_eq!(DifLcCtrlState::Rma.redundant_encoding(), 0x2739ce73);
    }

    #[test]
    fn lc_ctrl_token() {
        // This test assumes the system is little-endian.
        let token_bytes: [u8; 16] = [
            0x01, 0x02, 0x03, 0x04, // TOKEN_0
            0x11, 0x12, 0x13, 0x14, // TOKEN_1
            0x21, 0x22, 0x23, 0x24, // TOKEN_2
            0x31, 0x32, 0x33, 0x34, // TOKEN_3
        ];
        let token = DifLcCtrlToken::from(token_bytes);
        let words: [u32; 4] = token.into_register_values();
        assert_eq!(words, [0x04030201, 0x14131211, 0x24232221, 0x34333231]);
    }

    #[test]
    fn lc_ctrl_register_offsets() {
        assert_eq!(LcCtrlReg::LcState.byte_offset(), 0x34);
        assert_eq!(0x34 / 4, 0xd);
        assert_eq!(LcCtrlReg::LcState.word_offset(), 0xd);
    }

    #[test]
    fn lc_bit_union() {
        assert_eq!(LcBit::union([] as [LcCtrlStatusBit; 0]), 0);
        assert_eq!(LcBit::union([LcCtrlStatusBit::Initialized]), 1);
        assert_eq!(
            LcBit::union([LcCtrlStatusBit::Initialized, LcCtrlStatusBit::Ready]),
            3
        );
    }
}
