	.set noreorder

#include "pspstub.s"

	STUB_START "cushome",0x40090000,0x00020005
	STUB_FUNC  0x666404B9,cusHomeGetStatus
	STUB_FUNC  0x099CE3D7,cusHomeClose
	STUB_END
