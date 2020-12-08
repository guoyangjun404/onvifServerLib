PREFIX = arm-himix200-linux-

CC=$(PREFIX)gcc
STRIP=$(PREFIX)strip

CUR_PATH := $(shell pwd)

SYSLOGD=Syslogd
all: clean $(SYSLOGD)

CUR_PATH := $(shell pwd)

CFLAGS += -I$(SDUTILS_PATH)/include
LDFLAGS += -L$(SDUTILS_PATH)/libs 

$(SYSLOGD): syslogd.o
	$(CC)  -o $@ $^ $(CFLAGS) -Wl,-gc-sections  -O2 -Wall  $(LDFLAGS) -lpthread
	$(STRIP) $@	
#	cp $(SYSLOGD) $(INSTALL_DIR) -f

jz_syslogd.o: syslogd.c

clean:
	rm -f *.o $(SYSLOGD) 
#$(INSTALL_DIR)/$(SYSLOGD)

