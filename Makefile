# Compiler
CC = gcc

# Flags
FLAGS = -Wall
DFLAGS = -g -ggdb
RFLAGS = -mwindows -O3

# Source Files
SRC = src/*.c

# Include Directories
INC = -I include/ -I dep/glfw/include -I dep/glad/include

# Library Directories
LIB = -L dep/glfw/lib -L dep/glad/lib

# Linker 
LINK = -lglfw3 -lgdi32 -lopengl32 -lglad

# Build Directories
DDIR = bin/debug
RDIR = bin/release

# Build Name
BUILD = 2D_Lighting.exe

# Targets

all:
	make debug
	make release

test:
	make debug 
	cd $(DDIR) & $(BUILD)

debug: 
	$(CC) $(FLAGS) $(DFLAGS) $(SRC) $(INC) $(LIB) $(LINK) -o $(DDIR)/$(BUILD)

release:
	$(CC) $(FLAGS) $(RFLAGS) $(SRC) $(INC) $(LIB) $(LINK) -o $(RDIR)/$(BUILD)

