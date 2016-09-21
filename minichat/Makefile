
DIRS = common cert presence seq account profile addrbook msgbox logic

all:
	for d in $(DIRS); do \
		cd $$d; make; cd ..; \
	done

clean:
	for d in $(DIRS); do \
		cd $$d; make clean; cd ..; \
	done
