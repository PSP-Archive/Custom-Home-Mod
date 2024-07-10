PATH=C:\pspsdk\bin\;C:\pspsdk\psp\bin\
make clean
make > log.txt 2>&1
C:\pspsdk\psp\sdk\psp-packer\bin\psp-packer *.prx
del *.o
del *.elf
:: copy *.prx F:\seplugins\cushome\
start log.txt
PAUSE