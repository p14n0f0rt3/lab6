SHELL=cmd
PORTN=$(shell type COMPORT.inc)
CC=avr-gcc
CPU=-mmcu=atmega328p
COPT=-g -Os -Wall $(CPU)
OBJS=ADCtest.o usart.o

ADCtest.elf: $(OBJS)
	avr-gcc $(CPU) $(OBJS) -Wl,-u,vfprintf -lprintf_flt -lm -o ADCtest.elf
	avr-objcopy -j .text -j .data -O ihex ADCtest.elf ADCtest.hex
	@echo done!
	
ADCtest.o: ADCtest.c usart.h
	avr-gcc $(COPT) -c ADCtest.c

usart.o: usart.c usart.h
	avr-gcc $(COPT) -c usart.c

clean:
	@del *.hex *.elf $(OBJS) 2>nul

LoadFlash:
	@taskkill /f /im putty.exe /t /fi "status eq running" > NUL
	spi_atmega -p -v -crystal ADCtest.hex
	@cmd /c start putty -serial $(PORTN) -sercfg 115200,8,n,1,N

putty:
	@taskkill /f /im putty.exe /t /fi "status eq running" > NUL
	@cmd /c start putty -serial $(PORTN) -sercfg 115200,8,n,1,N

dummy: ADCtest.hex
	@echo Hello dummy!
	