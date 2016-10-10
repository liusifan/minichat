
include ../../../comm.mk

CFLAGS := $(COMMON_INCLUDE) $(CFLAGS)
LDFLAGS := -L$(MINICHAT_ROOT)/minichat/common -lcommon -L$(PHXRPC_ROOT)/lib -lphxrpc $(LDFLAGS)

LIB_OBJS = iOssAttrApi.o iOssAttr4Svr.o client_monitor_sk.o server_monitor_sk.o monitor_factory_sk.o logic_monitor_sk.o
TARGETS = libphxrpc_plugin_monitor_sk.a 

all: $(TARGETS) 

libphxrpc_plugin_monitor_sk.a: $(LIB_OBJS)
	$(AR) $@ $^
	mkdir -p ../../lib; cp $@ ../../lib 

clean:
	@( $(RM) $(TARGETS) )
	@( $(RM) *.o core.* $(LIB_OBJS) )
