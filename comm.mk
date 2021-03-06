
where-am-i = $(abspath $(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST)))

#$(warning $(dir $(call where-am-i)))

MINICHAT_ROOT := $(dir $(call where-am-i))

include $(MINICHAT_ROOT)/phxrpc/phxrpc.mk

ifeq ($(OS),Darwin)
	PLUGIN_REDIS_CONFIG_LOADER_LDFLAGS = \
			-Wl,-force_load,$(MINICHAT_ROOT)/minichat/lib/libphxrpc_plugin_redis_config_loader.a
	PLUGIN_SK_MONITOR_LDFLAGS = \
			-Wl,-force_load,$(MINICHAT_ROOT)/minichat/lib/libphxrpc_plugin_monitor_sk.a
else
	PLUGIN_REDIS_CONFIG_LOADER_LDFLAGS = -Wl,--whole-archive -L$(MINICHAT_ROOT)/minichat/lib/ \
			-lphxrpc_plugin_redis_config_loader -Wl,--no-whole-archive
	PLUGIN_SK_MONITOR_LDFLAGS = -Wl,--whole-archive -L$(MINICHAT_ROOT)/minichat/lib/ \
			-lphxrpc_plugin_monitor_sk -Wl,--no-whole-archive
endif

PLUGIN_REDIS_CONFIG_LOADER_LDFLAGS :=

COMMON_INCLUDE = -I$(MINICHAT_ROOT)/minichat/common/
COMMON_LDFLAGS = -L$(MINICHAT_ROOT)/minichat/common -lcommon 

YASSL_CFLAGS = -I$(MINICHAT_ROOT)/third_party/yassl -DUSE_SYS_STL -DSIZEOF_LONG_LONG=8
YASSL_LDFLAGS = -L$(MINICHAT_ROOT)/third_party/yassl/lib -ltaocrypt

REDIS_CLIENT_INCLUDE = -I$(MINICHAT_ROOT)/third_party/ \
  	-I$(MINICHAT_ROOT)/third_party/

REDIS_CLIENT_LIBRARY = -L$(MINICHAT_ROOT)/third_party/r3c/ -lr3c \
  	-L$(MINICHAT_ROOT)/third_party/hiredis -lhiredis

ACCOUNT_CFLAGS += $(REDIS_CLIENT_INCLUDE) -I$(MINICHAT_ROOT)/minichat

ACCOUNT_LDFLAGS := $(PLUGIN_ELPP_LDFLAGS) $(PLUGIN_SK_MONITOR_LDFLAGS) $(PLUGIN_REDIS_CONFIG_LOADER_LDFLAGS) \
	$(COMMON_LDFLAGS) \
	-L$(PHXRPC_ROOT)/lib -lphxrpc \
	$(LDFLAGS) \
	$(REDIS_CLIENT_LIBRARY)

ADDRBOOK_CFLAGS += $(REDIS_CLIENT_INCLUDE) -I$(MINICHAT_ROOT)/minichat

ADDRBOOK_LDFLAGS := $(PLUGIN_ELPP_LDFLAGS) $(PLUGIN_SK_MONITOR_LDFLAGS) $(PLUGIN_REDIS_CONFIG_LOADER_LDFLAGS) \
	$(COMMON_LDFLAGS) \
	-L$(MINICHAT_ROOT)/minichat/seq -lseq_client\
	-L$(PHXRPC_ROOT)/lib -lphxrpc \
	$(LDFLAGS) \
	$(REDIS_CLIENT_LIBRARY)

CERT_CFLAGS += $(REDIS_CLIENT_INCLUDE) -I$(MINICHAT_ROOT)/minichat $(YASSL_CFLAGS)

CERT_LDFLAGS :=  $(PLUGIN_ELPP_LDFLAGS) $(PLUGIN_SK_MONITOR_LDFLAGS) $(PLUGIN_REDIS_CONFIG_LOADER_LDFLAGS) \
	$(COMMON_LDFLAGS) \
	-L$(PHXRPC_ROOT)/lib -lphxrpc \
	-L$(MINICHAT_ROOT)/minichat/crypt -lminicrypt \
	$(YASSL_LDFLAGS) \
	$(LDFLAGS) \
	$(REDIS_CLIENT_LIBRARY)

LOGIC_CFLAGS += $(COMMON_INCLUDE) $(REDIS_CLIENT_INCLUDE) -I$(MINICHAT_ROOT)/minichat $(YASSL_CFLAGS)

LOGIC_LDFLAGS := $(PLUGIN_ELPP_LDFLAGS) $(PLUGIN_SK_MONITOR_LDFLAGS) $(PLUGIN_REDIS_CONFIG_LOADER_LDFLAGS) \
	$(COMMON_LDFLAGS) \
	-L$(MINICHAT_ROOT)/minichat/cert -lcert_client \
	-L$(MINICHAT_ROOT)/minichat/profile -lprofile_client \
	-L$(MINICHAT_ROOT)/minichat/account -laccount_client \
	-L$(MINICHAT_ROOT)/minichat/presence -lpresence_client \
	-L$(MINICHAT_ROOT)/minichat/msgbox -lmsgbox_client \
	-L$(MINICHAT_ROOT)/minichat/seq -lseq_client \
	-L$(MINICHAT_ROOT)/minichat/addrbook -laddrbook_client \
	-L$(MINICHAT_ROOT)/minichat/crypt -lminicrypt \
	$(YASSL_LDFLAGS) \
	-L$(PHXRPC_ROOT)/lib -lphxrpc $(LDFLAGS) $(REDIS_CLIENT_LIBRARY)

MSGBOX_CFLAGS += $(REDIS_CLIENT_INCLUDE) -I$(MINICHAT_ROOT)/minichat

MSGBOX_LDFLAGS := $(PLUGIN_ELPP_LDFLAGS) $(PLUGIN_SK_MONITOR_LDFLAGS) $(PLUGIN_REDIS_CONFIG_LOADER_LDFLAGS) \
	$(COMMON_LDFLAGS) \
	-L$(MINICHAT_ROOT)/minichat/seq -lseq_client \
	-L$(PHXRPC_ROOT)/lib -lphxrpc \
	$(LDFLAGS) \
	$(REDIS_CLIENT_LIBRARY)

PRESENCE_CFLAGS += $(REDIS_CLIENT_INCLUDE) -I$(MINICHAT_ROOT)/minichat

PRESENCE_LDFLAGS := $(PLUGIN_ELPP_LDFLAGS) $(PLUGIN_SK_MONITOR_LDFLAGS) $(PLUGIN_REDIS_CONFIG_LOADER_LDFLAGS) \
	$(COMMON_LDFLAGS) \
	-L$(PHXRPC_ROOT)/lib -lphxrpc \
	$(LDFLAGS) \
	$(REDIS_CLIENT_LIBRARY)

PROFILE_CFLAGS += $(REDIS_CLIENT_INCLUDE) -I$(MINICHAT_ROOT)/minichat

PROFILE_LDFLAGS := $(PLUGIN_ELPP_LDFLAGS) $(PLUGIN_SK_MONITOR_LDFLAGS) $(PLUGIN_REDIS_CONFIG_LOADER_LDFLAGS) \
	$(COMMON_LDFLAGS) \
	-L$(PHXRPC_ROOT)/lib -lphxrpc \
	$(LDFLAGS) \
	$(REDIS_CLIENT_LIBRARY)


SEQ_CFLAGS += $(REDIS_CLIENT_INCLUDE) -I$(MINICHAT_ROOT)/minichat

SEQ_LDFLAGS := $(PLUGIN_ELPP_LDFLAGS) $(PLUGIN_SK_MONITOR_LDFLAGS) $(PLUGIN_REDIS_CONFIG_LOADER_LDFLAGS) \
	$(COMMON_LDFLAGS) \
	-L$(PHXRPC_ROOT)/lib -lphxrpc \
	$(LDFLAGS) \
	$(REDIS_CLIENT_LIBRARY)


