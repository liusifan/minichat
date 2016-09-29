include ../../comm.mk

CFLAGS += $(REDIS_CLIENT_INCLUDE)

LDFLAGS := -L$(PHXRPC_ROOT)/lib -lphxrpc $(LDFLAGS) $(REDIS_CLIENT_LIBRARY)

LIBCOMMON_OBJS = redis_client_ex.o redis_client_factory.o

TARGETS = libcommon.a

all: $(TARGETS)

libcommon.a: $(LIBCOMMON_OBJS)
	$(AR) $@ $^

clean:
	@($(RM) $(TARGETS))
	@($(RM) *.o)
	@($(RM) phxrpc_*)

