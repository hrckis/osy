LIBNAME = compute
LIBFILE = lib$(LIBNAME).so

CPPFLAGS += -g -Wall -Wextra -fPIC

all: $(LIBFILE)

clean:
	rm -rf $(LIBFILE) *.o

$(LIBFILE): compute.o
	g++ -shared -o $@ $^