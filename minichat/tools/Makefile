
include ../../comm.mk

CFLAGS += $(LOGIC_CFLAGS)
LDFLAGS += -L$(MINICHAT_ROOT)/minichat/logic -llogic_client $(LOGIC_LDFLAGS)

TARGETS = init_tools sendmsg_tools sync_tools redis_tools test_exp

all: $(TARGETS)

init_tools: init_tools.o init_account.o init_profile.o init_addrbook.o
	$(LINKER) $^ $(LDFLAGS) -o $@

sendmsg_tools: benchmark.o sendmsg_tools.o 
	$(LINKER) $^ $(LDFLAGS) -o $@

sync_tools: sync_tools.o
	$(LINKER) $^ $(LDFLAGS) -o $@

redis_tools: redis_tools.o
	$(LINKER) $^ $(LDFLAGS) -o $@

test_exp: test_exp.o
	$(LINKER) $^ -lstdc++ -lm -o $@

clean:
	$(RM) *.o $(TARGETS)
