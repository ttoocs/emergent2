CC = g++

EXEFILE = template

IFLAGS= -I/usr/include/freetype2
LFLAGS= -L/usr/lib/nvidia-375 -L/usr/local/lib -L/usr/include/GL -L/usr/local/include/freetype2 -L/usr/local/lib/
LIBS = -lglfw -lGL -lGLU -lOpenGL -lGLEW -pthread -lfreetype

SRC=*.cpp
DEPS=*.h

$(EXEFILE): 
	$(CC) -std=c++11 -o $(EXEFILE) -Wall -Wno-comment $(SRC) $(IFLAGS) $(LFLAGS) $(LIBS) -Ofast

all: run clean

run: $(EXEFILE)
	./$(EXEFILE)
clean:
	rm $(EXEFILE)
