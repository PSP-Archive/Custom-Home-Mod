release: all
	psp-build-exports -s exports.exp

TARGET		=	cushome

BUILD_PRX = 1


OBJS	=	common.o menu.o states.o getname.o thread.o usb.o memory.o main.o stub.o exports.o image.o
OBJS	+=	pict/screenshot.o pict/file.o pict/pictdraw.o
OBJS	+=	syslibc/syslibc.o syslibc/qsort.o
OBJS	+=	cmlib/cmLibCtrl.o cmlib/cmlibAudio.o

PRX_EXPORTS	=	exports.exp
USE_KERNEL_LIBC	=	1
USE_KERNEL_LIBS	=	1

INCDIR	=	
CFLAGS	=	-Os -G 0 -Wall -fno-strict-aliasing
ASFLAGS	=	$(CFLAGS)

LIBDIR 	= 	
LDFLAGS	=	-mno-crt0 -nostartfiles
LIBS	=	-lpspsystemctrl_kernel -lpsppower -lpsprtc -lpspkubridge -lpspusb -lpspusbstor -lpspsysmem_kernel
LIBS	+=	libinip.a ./cmlib/libcmlibMenu.a ./cmlib/libcmlibConv.a


PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build_prx.mak

