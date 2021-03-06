include $(CRUNKY)/Settings.mk

VPATH=$(CRUNKY)

AOPS = --warn --fatal-warnings 
#COPS =  -nostdlib -nostartfiles -ffreestanding -I$(CRUNKY)

CR_OBJS = basal.o interrupts.o gpio.o lfb.o font.psf.o font.sfn.o \
          mbox.o memory.o mini_uart.o \
          nanolib-impl.o \
          sd.o uart0.o \
          timers.o 

CR_OBJS += vectors.o
CR_OBJS += bcm2835.o
CR_OBJS += fat.o
CR_OBJS += tat.o






$(IMG) : $(HEX)

LIB_CRUNKY = libcrunky.o

$(LIB_CRUNKY) : $(CRUNKY_OBJS)

all : $(IMG)

clean :
	rm -f *.o *.bin *.hex *.elf *.list *.img *.a $(BUILT)


%.o : %.c
	$(CC) $(COPS) -c $^ -o $@

%.o : %.cc
	$(CXX) $(CXXFLAGS) -c $^ -o $@

LINKER = $(CRUNKY)/linker.ld


NEWLIB = -L/usr/lib/arm-none-eabi/newlib/hard
NEWLIB += -lstdc++_nano  -lsupc++_nano
#NEWLIB += -lstdc++
#NEWLIB += -lsupc++
#NEWLIB += -lsupc++
NEWLIB += -lc_nano
#NEWLIB += -lg
NEWLIB += -lg_nano
GCCLIB = -L/usr/lib/gcc/arm-none-eabi/7.3.1/hard -lgcc

LD_CRUNKY = -L$(CRUNKY) -lcrunky -m

#LIBS = $(NEWLIB) $(LD_CRUNKY) $(GCCLIB) -lm
#LIBS = $(LD_CRUNKY) $(NEWLIB) $(GCCLIB) -lm
LIBS =  $(NEWLIB) $(GCCLIB) -lm

$(ELF) : $(LINKER) $(CR_OBJS) $(OBJS)  $(LIBUSPI) 
	$(LD)    $(CR_OBJS) $(OBJS) -T $(LINKER)  $(LIBS) -o $@
	$(OBJDUMP) -D $@ > $(KERNEL).list

$(IMG) : $(ELF)
	$(OBJCOPY) $(ELF) -O binary $@

$(HEX) : $(ELF)
	$(OBJCOPY) $^ -O ihex $(HEX)

flash : install

install:
	#cp $(IMG) /media/pi/50BD-6FEC
	cp $(IMG) /media/pi/boot
