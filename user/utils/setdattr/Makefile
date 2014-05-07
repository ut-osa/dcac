.PHONY: all clean install

TARGETS=setdattr

all: $(TARGETS)

setdattr: setdattr.o

clean:
	rm -f *.o
	rm -f $(TARGETS)

install: $(TARGETS)
	mkdir -p ../../bin
	cp $(TARGETS) ../../bin
