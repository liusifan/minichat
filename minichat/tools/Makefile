
include ../../comm.mk

CFLAGS += $(LOGIC_CFLAGS)
LDFLAGS += $(LOGIC_LDFLAGS)

TARGETS = init_tools test_exp

all: $(TARGETS)

init_tools: init_tools.o init_account.o init_profile.o init_addrbook.o
	$(LINKER) $^ -L../account -laccount_client -lpthread $(LDFLAGS) -o $@

test_exp: test_exp.o
	$(LINKER) $^ -lstdc++ -o $@

clean:
	$(RM) *.o $(TARGETS)
