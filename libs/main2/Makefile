TARGET  = main2
LIBDIR  = ../dyn_lib1
LIBNAME = compute

CPPFLAGS += -g -Wall -Wextra -I$(LIBDIR)
LDFLAGS  += -L$(LIBDIR)
LDLIBS   += -l$(LIBNAME)

all: $(TARGET)

clean:
	rm -rf $(TARGET) *.o

$(TARGET): main2.cpp
	g++ $(CPPFLAGS) $(LDFLAGS) $< $(LDLIBS) -o $@