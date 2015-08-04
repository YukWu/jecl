DIR_SRC = ../src
DIR_INC = ../include
DIR_OBJ = ../obj
DIR_BIN = ../bin

SRC = $(wildcard $(DIR_SRC)/*.cpp)
OBJ = $(patsubst %.cpp,%.o,$(notdir $(SRC)))
TARGET = timer
BIN_TARGET = $(TARGET)

CC = g++
CFLAGS = -g -I ../include
SUFIX = cpp

$(BIN_TARGET):timer.o $(OBJ)
	$(CC) $^ -o $@ -lpthread
pool:testmain.o $(OBJ)
	$(CC) $^ -o $@ -lpthread
testmain.o:testmain.cpp
	$(CC) $(CFLAGS) -c $< -o $@ -lpthread
timer.o:TimerTest.cpp
	$(CC) $(CFLAGS) -c $< -o $@ -lphtread
%.o:$(DIR_SRC)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@ -lpthread

