
include ../../comm.mk

include ../../phxrpc/phxrpc.mk

CFLAGS += -I$(MINICHAT_ROOT)/third_party/yassl -DUSE_SYS_STL

LDFLAGS += -L$(MINICHAT_ROOT)/third_party/yassl/lib -ltaocrypt

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

