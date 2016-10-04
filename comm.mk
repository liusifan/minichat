
where-am-i = $(abspath $(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST)))

#$(warning $(dir $(call where-am-i)))

MINICHAT_ROOT := $(dir $(call where-am-i))

include $(MINICHAT_ROOT)/phxrpc/phxrpc.mk

REDIS_CLIENT_INCLUDE = -I$(MINICHAT_ROOT)/third_party/ \
  	-I$(MINICHAT_ROOT)/third_party/

REDIS_CLIENT_LIBRARY = -L$(MINICHAT_ROOT)/third_party/r3c/ -lr3c \
  	-L$(MINICHAT_ROOT)/third_party/hiredis -lhiredis


ACCOUNT_CFLAGS += $(REDIS_CLIENT_INCLUDE) -I$(MINICHAT_ROOT)/minichat

ACCOUNT_LDFLAGS := $(PLUGIN_ELPP_LDFLAGS) $(PLUGIN_SK_MONITOR_LDFLAGS) $(PLUGIN_REDIS_CONFIG_LOADER_LDFLAGS) \
	-L$(MINICHAT_ROOT)/minichat/common -lcommon \
	-L$(PHXRPC_ROOT)/lib -lphxrpc \
	$(LDFLAGS) \
	$(REDIS_CLIENT_LIBRARY)

ADDRBOOK_CFLAGS += $(REDIS_CLIENT_INCLUDE) -I$(MINICHAT_ROOT)/minichat

ADDRBOOK_LDFLAGS := $(PLUGIN_ELPP_LDFLAGS) $(PLUGIN_SK_MONITOR_LDFLAGS) $(PLUGIN_REDIS_CONFIG_LOADER_LDFLAGS) \
	-L$(MINICHAT_ROOT)/minichat/common -lcommon \
	-L$(MINICHAT_ROOT)/minichat/seq -lseq_client\
	-L$(PHXRPC_ROOT)/lib -lphxrpc \
	$(LDFLAGS) \
	$(REDIS_CLIENT_LIBRARY)

CERT_CFLAGS += $(REDIS_CLIENT_INCLUDE) -I$(MINICHAT_ROOT)/minichat \
		  -I$(MINICHAT_ROOT)/third_party/yassl -DUSE_SYS_STL

CERT_LDFLAGS :=  $(PLUGIN_ELPP_LDFLAGS) $(PLUGIN_SK_MONITOR_LDFLAGS) $(PLUGIN_REDIS_CONFIG_LOADER_LDFLAGS) \
	-L$(PHXRPC_ROOT)/lib -lphxrpc \
	-L$(MINICHAT_ROOT)/minichat/crypt -lminicrypt \
	-L$(MINICHAT_ROOT)/third_party/yassl/lib -ltaocrypt \
	$(LDFLAGS) \
	$(REDIS_CLIENT_LIBRARY)

LOGIC_CFLAGS += $(REDIS_CLIENT_INCLUDE) -I$(MINICHAT_ROOT)/minichat \
		  -I$(MINICHAT_ROOT)/third_party/yassl -DUSE_SYS_STL

LOGIC_LDFLAGS := $(PLUGIN_ELPP_LDFLAGS) $(PLUGIN_SK_MONITOR_LDFLAGS) $(PLUGIN_REDIS_CONFIG_LOADER_LDFLAGS) \
	-L$(MINICHAT_ROOT)/minichat/cert -lcert_client \
	-L$(MINICHAT_ROOT)/minichat/profile -lprofile_client \
	-L$(MINICHAT_ROOT)/minichat/account -laccount_client \
	-L$(MINICHAT_ROOT)/minichat/presence -lpresence_client \
	-L$(MINICHAT_ROOT)/minichat/msgbox -lmsgbox_client \
	-L$(MINICHAT_ROOT)/minichat/seq -lseq_client \
	-L$(MINICHAT_ROOT)/minichat/addrbook -laddrbook_client \
	-L$(MINICHAT_ROOT)/minichat/crypt -lminicrypt \
	-L$(MINICHAT_ROOT)/third_party/yassl/lib -ltaocrypt \
	-L$(PHXRPC_ROOT)/lib -lphxrpc $(LDFLAGS) $(REDIS_CLIENT_LIBRARY)

MSGBOX_CFLAGS += $(REDIS_CLIENT_INCLUDE) -I$(MINICHAT_ROOT)/minichat

MSGBOX_LDFLAGS := $(PLUGIN_ELPP_LDFLAGS) $(PLUGIN_SK_MONITOR_LDFLAGS) $(PLUGIN_REDIS_CONFIG_LOADER_LDFLAGS) \
	-L$(MINICHAT_ROOT)/minichat/common -lcommon \
	-L$(MINICHAT_ROOT)/minichat/seq -lseq_client \
	-L$(PHXRPC_ROOT)/lib -lphxrpc \
	$(LDFLAGS) \
	$(REDIS_CLIENT_LIBRARY)

PRESENCE_CFLAGS += $(REDIS_CLIENT_INCLUDE) -I$(MINICHAT_ROOT)/minichat

PRESENCE_LDFLAGS := $(PLUGIN_ELPP_LDFLAGS) $(PLUGIN_SK_MONITOR_LDFLAGS) $(PLUGIN_REDIS_CONFIG_LOADER_LDFLAGS) \
	-L$(MINICHAT_ROOT)/minichat/common -lcommon \
	-L$(PHXRPC_ROOT)/lib -lphxrpc \
	$(LDFLAGS) \
	$(REDIS_CLIENT_LIBRARY)

PROFILE_CFLAGS += $(REDIS_CLIENT_INCLUDE) -I$(MINICHAT_ROOT)/minichat

PROFILE_LDFLAGS := $(PLUGIN_ELPP_LDFLAGS) $(PLUGIN_SK_MONITOR_LDFLAGS) $(PLUGIN_REDIS_CONFIG_LOADER_LDFLAGS) \
	-L$(MINICHAT_ROOT)/minichat/common -lcommon \
	-L$(PHXRPC_ROOT)/lib -lphxrpc \
	$(LDFLAGS) \
	$(REDIS_CLIENT_LIBRARY)


SEQ_CFLAGS += $(REDIS_CLIENT_INCLUDE) -I$(MINICHAT_ROOT)/minichat

SEQ_LDFLAGS := $(PLUGIN_ELPP_LDFLAGS) $(PLUGIN_SK_MONITOR_LDFLAGS) $(PLUGIN_REDIS_CONFIG_LOADER_LDFLAGS) \
	-L$(MINICHAT_ROOT)/minichat/common -lcommon \
	-L$(PHXRPC_ROOT)/lib -lphxrpc \
	$(LDFLAGS) \
	$(REDIS_CLIENT_LIBRARY)


