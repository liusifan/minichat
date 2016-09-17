
where-am-i = $(abspath $(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST)))

#$(warning $(dir $(call where-am-i)))

MINICHAT_ROOT := $(dir $(call where-am-i))

REDIS_CLIENT_INCLUDE = -I$(MINICHAT_ROOT)/third_party/ \
  	-I$(MINICHAT_ROOT)/third_party/

REDIS_CLIENT_LIBRARY = -L$(MINICHAT_ROOT)/third_party/r3c/ -lr3c \
  	-L$(MINICHAT_ROOT)/third_party/hiredis -lhiredis

