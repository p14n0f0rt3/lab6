SHELL=cmd
OBJS=AVR_Receiver.o uart.o
PORTN=$(shell type COMPORT.inc)

AVR_Receiver.elf: $(OBJS)
	avr-gcc -mmcu=atmega328p $(OBJS) -o AVR_Receiver.elf
	avr-objcopy -j .text -j .data -O ihex AVR_Receiver.elf AVR_Receiver.hex
	@echo done!
	
AVR_Receiver.o: AVR_Receiver.c uart.h
	avr-gcc -g -Os -Wall -mmcu=atmega328p -c AVR_Receiver.c

uart.o: uart.c uart.h
	avr-gcc -g -Os -Wall -mmcu=atmega328p -c uart.c

clean:
	@del *.hex *.elf *.o 2>nul

FlashLoad:
	@taskkill /f /im putty.exe /t /fi "status eq running" > NUL
	spi_atmega -p -v -crystal AVR_Receiver.hex

putty:
	@taskkill /f /im putty.exe /t /fi "status eq running" > NUL
	@cmd /c start putty -serial $(PORTN) -sercfg 115200,8,n,1,N

dummy: AVR_Receiver.hex
	@echo Hello dummy!

explorer:
	cmd /c start explorer .