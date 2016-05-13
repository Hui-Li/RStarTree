CC      = g++
CFLAGS  = -c -w -Wall -O2 -D_LINUX -std=c++0x
LDFLAGS =
LDADD   = -lrt
SOURCES = rstree/GenericTool.cpp rstree/mt19937ar.cpp rstree/PageFile.cpp rstree/main.cpp rstree/TimerTool.cpp rstree/ConfigTool.cpp
OBJECTS = $(SOURCES:.cpp=.o)

all: rst

rst: $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o rst $(LDADD)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

.cc.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf rstree/*.o
	rm -rf rst
