
include ../../../comm.mk

LIB_OBJS = redis_config_loader.o 

TARGETS = libphxrpc_plugin_redis_config_loader.a add_conf_tools

REDIS_CLIENT_INCLUDE = -I$(MINICHAT_ROOT)/third_party/hiredis \
  	-I$(MINICHAT_ROOT)/third_party/

REDIS_CLIENT_LIBRARY = -L$(MINICHAT_ROOT)/third_party/r3c/ -lr3c \
  	-L$(MINICHAT_ROOT)/third_party/hiredis -lhiredis

CFLAGS := -I$(MINICHAT_ROOT)/minichat/ -I$(MINICHAT_ROOT)/third_party $(REDIS_CLIENT_INCLUDE) $(CFLAGS)

LDFLAGS := -L$(MINICHAT_ROOT)/minichat/common -lcommon \
	-L$(PHXRPC_ROOT)/lib $(REDIS_CLIENT_LIBRARY) -lphxrpc $(LDFLAGS)

all: $(TARGETS) 

libphxrpc_plugin_redis_config_loader.a: $(LIB_OBJS)
	$(AR) $@ $^
	mkdir -p ../../lib; cp $@ ../../lib 

add_conf_tools: add_conf_tools.o 
	$(LINKER) $^  $(LDFLAGS) -o $@

clean:
	@( $(RM) $(TARGETS) )
	@( $(RM) *.o core.* $(LIB_OBJS) )
