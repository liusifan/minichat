# Makefile
#
# Generated by phxrpc_pb2server from account.proto
#
#

include ../../comm.mk

CFLAGS += $(ACCOUNT_CFLAGS) 
LDFLAGS += $(ACCOUNT_LDFLAGS)

#choose to use boost for network
#LDFLAGS := $(PLUGIN_BOOST_LDFLAGS) $(LDFLAGS)

SVR_OBJS = account.pb.o \
		   account_service_impl.o \
		   phxrpc_account_service.o \
		   phxrpc_account_dispatcher.o \
		   account_server_config.o \
		   account_main.o \
		   account_dao.o

CLI_OBJS = account.pb.o \
		   account_client.o \
		   phxrpc_account_stub.o

TARGETS = libaccount_client.a account-main account_tool_main

all: $(TARGETS)

account-main: $(SVR_OBJS)
	$(LINKER) $^ $(LDFLAGS) -o $@

libaccount_client.a: $(CLI_OBJS)
	$(AR) $@ $^

account_tool_main: phxrpc_account_tool.o account_tool_impl.o account_tool_main.o
	$(LINKER) $^ -L. -laccount_client $(LDFLAGS) -o $@

########## message ##########

account.pb.cc: account.pb.h

account.pb.h: account.proto
	$(PROTOBUF_ROOT)/bin/protoc -I$(PROTOBUF_ROOT)/include --cpp_out=. -I$(PHXRPC_ROOT) -I. $^

########## client ##########

phxrpc_account_stub.cpp: phxrpc_account_stub.h
phxrpc_account_stub.o: phxrpc_account_stub.h
account_client.cpp: phxrpc_account_stub.h
account_client.o: phxrpc_account_stub.h

phxrpc_account_stub.h: account.proto
	$(PHXRPC_ROOT)/codegen/phxrpc_pb2client $(PBFLAGS) -f $^ -d .

########## service ##########

phxrpc_account_service.cpp: phxrpc_account_service.h
phxrpc_account_service.o: phxrpc_account_service.h
account_service_impl.cpp: phxrpc_account_service.h
account_service_impl.o: phxrpc_account_service.h
phxrpc_account_dispatcher.cpp: phxrpc_account_service.h
phxrpc_account_dispatcher.o: phxrpc_account_service.h

phxrpc_account_service.h: account.proto
	$(PHXRPC_ROOT)/codegen/phxrpc_pb2service $(PBFLAGS) -f $^ -d .

########## tool ##########

phxrpc_account_tool.cpp: phxrpc_account_tool.h
phxrpc_account_tool.o: phxrpc_account_tool.h
account_tool_impl.cpp: phxrpc_account_tool.h
account_tool_impl.o: phxrpc_account_tool.h
account_tool_main.cpp: phxrpc_account_tool.h
account_tool_main.o: phxrpc_account_tool.h

phxrpc_account_tool.h: account.proto
	$(PHXRPC_ROOT)/codegen/phxrpc_pb2tool $(PBFLAGS) -f $^ -d .

clean:
	@($(RM) $(TARGETS))
	@($(RM) *.o)
	@($(RM) phxrpc_*)
	@($(RM) *.pb.*)

