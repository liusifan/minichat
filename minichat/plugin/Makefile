
DIRS = plugin_redis_config_loader plugin_sk

all:
	for d in $(DIRS); do \
		cd $$d; make; cd -; \
	done

clean:
	for d in $(DIRS); do \
		cd $$d; make clean; cd -; \
	done
