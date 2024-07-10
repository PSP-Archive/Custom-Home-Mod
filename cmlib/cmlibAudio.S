	.set noreorder

#include "pspstub.s"

	STUB_START "cmlibAudio",0x40090000,0x000A0005
	STUB_FUNC  0xF5BCBCE4,libaudioGetVolume
	STUB_FUNC  0xDB8C37B5,libaudioSetVolume
	STUB_FUNC  0x281D1BA3,libaudioMuteOn
	STUB_FUNC  0xDB253A1A,libaudioMuteOff
	STUB_FUNC  0xC096D1A7,libaudioMuteGetStatus
	STUB_FUNC  0x55D33ED4,libaudioMuteSetAgain
	STUB_FUNC  0x05B5AF43,libaudioIoEnable
	STUB_FUNC  0x6ED876CB,libaudioIoDisable
	STUB_FUNC  0x74494A09,libaudioIoGetStatus
	STUB_FUNC  0xF0A9A317,libaudioIoSetAgain
	STUB_END
