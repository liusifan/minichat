
include ../../comm.mk

CFLAGS += $(YASSL_CFLAGS)

LDFLAGS += $(YASSL_LDFLAGS) -L$(PHXRPC_ROOT)/lib/ -lphxrpc

LIBCRYPT_OBJS = pem_file.o crypt_utils.o

TARGETS = libminicrypt.a test_crypt_utils

all: $(TARGETS)

libminicrypt.a: $(LIBCRYPT_OBJS)
	$(AR) $@ $^

test_crypt_utils: test_crypt_utils.o
	$(LINKER) $^ -L. -lminicrypt $(LDFLAGS) -o $@

clean:
	@($(RM) $(TARGETS))
	@($(RM) *.o)
	@($(RM) phxrpc_*)

