TARGET  = main1
LIBDIR  = ../static_lib
LIBNAME = generator
LIBFILE = $(LIBDIR)/lib$(LIBNAME).a

CPPFLAGS += -g -Wall -Werror -I$(LIBDIR)
LDFLAGS  += -L$(LIBDIR)
LDLIBS   += -l$(LIBNAME)

all: $(TARGET)

clean:
	rm -rf $(TARGET) *.o

$(TARGET): main.cpp $(LIBFILE)
	g++ $(CPPFLAGS) $(LDFLAGS) $< $(LDLIBS) -o $@