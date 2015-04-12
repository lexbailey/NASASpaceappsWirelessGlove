LIBS = -lGL -lGLU -lglut -lm

all: glove


%.o: %.cpp
	g++ $^ -c -o $@

%.o: %.c
	g++ $^ -c -o $@

glove: glove.o rs232.o gears.o
	g++ $^ -o $@ ${LIBS}
