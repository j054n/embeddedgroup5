# MPLAB IDE generated this makefile for use with GNU make.
# Project: Board_Test_AD.mcp
# Date: Wed Oct 08 09:10:27 2008

AS = mpasmwin.exe
CC = mcc18.exe
LD = mplink.exe
AR = mplib.exe
RM = rm

Board_Test_AD.cof : main.o
	$(LD) /l"C:\MCC18\lib" "..\..\..\MCC18\lkr\18f2680i.lkr" "main.o" /z__MPLAB_BUILD=1 /o"Board_Test_AD.cof" /M"Board_Test_AD.map" /W

main.o : main.c ../../../MCC18/h/portb.h ../../../MCC18/h/delays.h ../../../MCC18/h/adc.h main.c ../../../MCC18/h/p18cxxx.h ../../../MCC18/h/p18f2680.h ../../../MCC18/h/pconfig.h
	$(CC) -p=18F2680 "main.c" -fo="main.o" -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-

clean : 
	$(RM) "main.o" "Board_Test_AD.cof" "Board_Test_AD.hex"

