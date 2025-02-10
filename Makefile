CC := $(CROSS_COMPILE)gcc


#CFLAGS += -Wall -Wextra
CFLAGS += -I/home/ithai/learning/gateway
#ifdef CROSS_COMPILE
#CFLAGS += --sysroot=$(TOOLCHAIN_DIR)/sysroot
#endif
#LDFLAGS += -lpaho-mqtt3c -lcurl -lcrypto


SRCS += $(shell find app -type f -name "*.c")
SRCS += $(shell find daemon -type f -name "*.c")
SRCS += $(shell find ota -type f -name "*.c")
SRCS += $(shell find thirdparty -type f -name "*.c")

OBJS = $(SRCS:.c=.o)

testBuffer: $(OBJS) test/testBuffer.o
	-@$(CC) $(CFLAGS) -o $@ $^
	-@./$@
	-@$(RM) $@ $^
