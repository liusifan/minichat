include ../../comm.mk

CFLAGS += $(REDIS_CLIENT_INCLUDE)

LDFLAGS := -L$(PHXRPC_ROOT)/lib -lphxrpc $(LDFLAGS) $(REDIS_CLIENT_LIBRARY)

LIBCOMMON_OBJS = redis_client_ex.o redis_client_factory.o \
				 push_client.o push_client_factory.o

TARGETS = libcommon.a test_push_client

all: $(TARGETS)

libcommon.a: $(LIBCOMMON_OBJS)
	$(AR) $@ $^

test_push_client: test_push_client.o
	$(LINKER) $^ -L. -lcommon $(LDFLAGS) -o $@

clean:
	@($(RM) $(TARGETS))
	@($(RM) *.o)
	@($(RM) phxrpc_*)

