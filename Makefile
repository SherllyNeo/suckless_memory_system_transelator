sltranselator: memory_system_transelator.c config.h 
	$(CC) memory_system_transelator.c config.h -g -std=gnu11 -o sltranselator -lm
install:
	cp -f sltranselator /usr/local/bin
clean:
	$(CC) memory_system_transelator.c config.h -g -std=gnu11 -o sltranselator -lm && rm vg*
