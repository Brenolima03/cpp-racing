# A simple Makefile for compiling and running small SDL projects

# Set the compiler
CC := clang++

# Set the compiler flags
CFLAGS := `sdl2-config --cflags` -ggdb3 -O0 --std=c++17 -Wall
LDFLAGS := `sdl2-config --libs` -lSDL2_image -lSDL2_ttf -lm

# Add source files here
SRCS := main.cpp car.cpp obstacle.cpp car_obstacle.cpp barrier_obstacle.cpp

# Generate names of object files
OBJS := $(SRCS:.cpp=.o)

# Name of executable
EXEC := main

# Default recipe
all: $(EXEC)

# Recipe for building the final executable
$(EXEC): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

# Recipe for building object files
%.o: %.cpp
	$(CC) -c $< -o $@ $(CFLAGS)

# Recipe to compile, run, and clean
run: $(EXEC)
	./$(EXEC)
	$(MAKE) clean

# Recipe to clean the workspace
clean:
	rm -f $(EXEC) $(OBJS)

.PHONY: all clean run
