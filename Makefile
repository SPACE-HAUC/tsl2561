CC=gcc
RM= rm -vf

EDCFLAGS:= -O2 -Wall -std=gnu11 -I ./ -I include/ -I drivers/ $(CFLAGS) $(DEBUG)
EDLDFLAGS:= -lm -lpapi -lpthread $(EDLDFLAGS)

all: EDCFLAGS+= -DUNIT_TEST_SINGLE

BUILDDRV=drivers/i2cbus/i2cbus.o \
	drivers/tca9458a/tca9458a.o

BUILDOBJS=$(BUILDDRV) \
tsl2561.o

TARGET=lux_tester.out

all: $(TARGET) 

test: test.o $(BUILDOBJS)
	$(CC) $< $(BUILDOBJS) -o $@.out $(LINKOPTIONS) \
	$(EDLDFLAGS)

$(TARGET): $(BUILDOBJS)
	$(CC) $(BUILDOBJS) $(EDCFLAGS) $(LINKOPTIONS) -o $@ \
	$(EDLDFLAGS)

%.o: %.c
	$(CC) $(EDCFLAGS) $(EDDEBUG) -o $@ -c $<

.PHONY: clean

clean:
	$(RM) $(BUILDOBJS)
	$(RM) $(TARGET)

spotless: clean

