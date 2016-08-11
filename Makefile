CPP = g++
LD = g++
CPPFLAGS = -g -O0 -std=c++11
LDFLAGS =
TARGET = hw3
OBJS = main.o TerrainGrid.o
OS = $(shell uname)

ifeq ($(OS),Darwin)
	LIB =  -framework OpenGL -framework Cocoa -lGLEW -lglfw3
else
	LIB =  -lGL -lGLEW -lglfw -L/opt/local/lib -lSOIL

endif

default: $(OBJS)
	$(LD) $(OBJS) $(LDFLAGS) $(LIB) -o $(TARGET)

main.o: main.cpp
	$(CPP) -c $(CPPFLAGS) main.cpp

terraingrid.o: TerrainGrid.cpp
	$(CPP) -c $(CPPFLAGS) TerrainGrid.cpp


clean:
	rm -f *.o $(TARGET)
