CC=g++
LIBS= -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_contrib
FLAGS= -std=c++11 -Wall -pedantic -DDEBUG -g
EXE=CAV.exe

objects=main.o

all: $(objects)
	$(CC) -o $(EXE) $^ $(LIBS)
	\rm *.o

%.o: %.cpp %.h
	$(CC) -c $< $(FLAGS)

main.o: main.cpp
	$(CC) -c $< $(FLAGS)

.PHONY: clean

clean:
	rm -rf *.o
	rm -rf $(PROJECT_NAME)
