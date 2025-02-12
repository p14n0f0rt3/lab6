SHELL=cmd
PORTN=$(shell type COMPORT.inc)
CC=avr-gcc
CPU=-mmcu=atmega328p
COPT=-g -Os -Wall $(CPU)
OBJS=Robot_Base.o usart.o

Robot_Base.elf: $(OBJS)
	avr-gcc $(CPU) $(OBJS) -o Robot_Base.elf
	avr-objcopy -j .text -j .data -O ihex Robot_Base.elf Robot_Base.hex
	@echo done!
	
Robot_Base.o: Robot_Base.c usart.h
	avr-gcc $(COPT) -c Robot_Base.c

usart.o: usart.c usart.h
	avr-gcc $(COPT) -c usart.c

clean:
	@del *.hex *.elf $(OBJS) 2>nul

LoadFlash:
	@taskkill /f /im putty.exe /t /fi "status eq running" > NUL
	spi_atmega -p -v -crystal Robot_Base.hex
	@cmd /c start putty -serial $(PORTN) -sercfg 115200,8,n,1,N

putty:
	@taskkill /f /im putty.exe /t /fi "status eq running" > NUL
	@cmd /c start putty -serial $(PORTN) -sercfg 115200,8,n,1,N

dummy: Robot_Base.hex
	@echo Hello dummy!
	