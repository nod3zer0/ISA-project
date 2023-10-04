TARGET := isa-ldapserver
SOURCES := $(wildcard src/*.c src/*.cpp)
OBJECTS := $(patsubst src%,obj%, $(patsubst %.c,%.o, $(patsubst %.cpp,%.o,$(SOURCES))))

INCLUDE := -I.
LIBPATH :=
LIBS :=

FLAGS :=  -Wall -g
CCFLAGS := $(FLAGS) --std=c++20 -lstdc++
CXXFLAGS := $(FLAGS)

CC := gcc
Cxx := g++

all: $(OBJECTS)
	$(CC) $(CCFLAGS) $(INCLUDE) $(OBJECTS) -o $(TARGET) $(LIBPATH) $(LIBS)

%.o: ../src/%.c
	$(CC) $(CCFLAGS) $(INCLUDE) -c $< -o $@

%.o: ../src/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

.PHONY: clean help

clean:
	rm -rf obj/*
	rm -f $(TARGET)

#build:	isa-ldapserver.cpp
#	c++ --std=c++20 -Wall -g isa-ldapserver.cpp -o isa-ldapserver
#	c++ --std=c++20 -Wall -g buildMessage.cpp -o buildMessage

#clean:
#	rm isa-ldapserver


