PROJECT = app
BUILD_DIR = build

#INCLUDES += ../mal.h
VPATH += ..
CFILES = main.c mal.c

#DEVICE=stm32f407vgt6
DEVICE=stm32f411re
OOCD_FILE = board/$(DEVICE).cfg

# You shouldn't have to edit anything below here.
#VPATH += $(SHARED_DIR)
#INCLUDES += $(patsubst %,-I%, . $(SHARED_DIR))
#OPENCM3_DIR=../libopencm3


include $(OPENCM3_DIR)/mk/genlink-config.mk
include ../rules.mk
include $(OPENCM3_DIR)/mk/genlink-rules.mk
