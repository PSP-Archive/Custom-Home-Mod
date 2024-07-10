	.set noreorder

#include "pspstub.s"

	STUB_START "cmlibCtrl",0x40090000,0x000A0005
	STUB_FUNC  0xD5D3042B,libctrlMaskAllButtonOn
	STUB_FUNC  0x36823094,libctrlMaskAllButtonOff
	STUB_FUNC  0x04D833DB,libctrlMaskAllButtonStatus
	STUB_FUNC  0xDE6B3EED,libctrlMaskAllButtonAgain
	STUB_FUNC  0xCBA5391D,libctrlMaskButtonOn
	STUB_FUNC  0x88814A36,libctrlMaskButtonOff
	STUB_FUNC  0x9E80CCEC,libctrlMaskButtonStatus
	STUB_FUNC  0x490D9E7B,libctrlCountButtons
	STUB_FUNC  0xDB6A1035,libctrlWaitButtonUp
	STUB_FUNC  0xD76D3495,libctrlWaitButtonDown
	STUB_END
