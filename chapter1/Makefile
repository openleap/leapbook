# change this to point at the location of your Leap_SDK directory
LEAPPATH=../../../Leap_SDK

SOURCES=quickstart.cpp
EXECUTABLE=quickstart
OBJECTS=$(SOURCES:.cpp=.o)
CFLAGS=-I$(LEAPPATH)/include
LDFLAGS=-lLeap -L$(LEAPPATH)/lib/libc++

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	g++ $(LDFLAGS) $(CFLAGS) $(OBJECTS) -o $@

.cpp.o:
	g++ -c $(CFLAGS) $< -o $@

clean:
	rm -rf *.o quickstart
