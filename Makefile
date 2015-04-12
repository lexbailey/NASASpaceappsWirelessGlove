LIBS = -lfreeglut  -lGLU32 -lopengl32 -lm

all: glove.exe


%.o: %.cpp
	g++ $^ -c -o $@

%.o: %.c
	g++ $^ -c -o $@

glove.exe: glove.o rs232.o gears.o
	g++ $^ -o $@ ${LIBS}