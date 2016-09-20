
DIRS = common cert account addrbook msgbox seq logic

all:
	for d in $(DIRS); do \
		cd $$d; make; cd ..; \
	done

clean:
	for d in $(DIRS); do \
		cd $$d; make clean; cd ..; \
	done
