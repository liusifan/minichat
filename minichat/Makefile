
DIRS = common account addrbook msgbox seq logicsvr

all:
	for d in $(DIRS); do \
		cd $$d; make; cd ..; \
	done

clean:
	for d in $(DIRS); do \
		cd $$d; make clean; cd ..; \
	done
