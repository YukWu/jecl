DIR_SRC = ../src
DIR_INC = ../include
DIR_OBJ = ../obj
DIR_BIN = ../bin

SRC = $(wildcard $(DIR_SRC)/*.cpp)
OBJ = $(patsubst %.cpp,%.o,$(notdir $(SRC)))
TARGET = main
BIN_TARGET = $(TARGET)

CC = clang++
CFLAGS = -g -I ../include
SUFIX = cpp

$(BIN_TARGET):testmain.o $(OBJ)
	$(CC) $^ -o $@
testmain.o:TimerTest.cpp
	$(CC) $(CFLAGS) -c $< -o $@
%.o:$(DIR_SRC)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@ -lpthread

