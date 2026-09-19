LIBNAME = generator
LIBFILE = lib$(LIBNAME).a

CPPFLAGS += -g -Wall -Werror

all: $(LIBFILE)

clean:
	rm -rf $(LIBFILE) *.o

$(LIBFILE): generator.o
	ar r $@ $^