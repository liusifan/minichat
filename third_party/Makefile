
LIBS = libr3c.a

all: $(LIBS)

libr3c.a: r3c/r3c.o r3c/crc16.o
	ar cru r3c/$@ $^

r3c/r3c.o: r3c/r3c.cpp
	gcc -c -g2 $< -I. -o $@ 

r3c/crc16.o: r3c/crc16.cpp
	gcc -c -g2 $< -I. -o $@ 

clean:
	rm -f r3c/*.o
