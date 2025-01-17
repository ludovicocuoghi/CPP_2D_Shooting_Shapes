# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++20 -Wall -Wextra -I/opt/homebrew/opt/sfml@2/include \
           -I./src/imgui -I./src/imgui-sfml -I./src/

# SFML library flags
LDFLAGS = -L/opt/homebrew/opt/sfml@2/lib -lsfml-graphics -lsfml-window -lsfml-system -framework OpenGL

# Target executable
TARGET = bin/sfml_app

# Source files
SRC = main.cpp src/Game.cpp \
      $(wildcard src/imgui/*.cpp) $(wildcard src/imgui-sfml/*.cpp)

# Object files directory
OBJ_DIR = build

# Object files (convert source file names to object files in the build directory)
OBJ = $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(SRC))

# Default target
all: $(TARGET)

# Link object files into the final executable
$(TARGET): $(OBJ)
	@mkdir -p $(dir $@) # Ensure the bin directory exists
	$(CXX) $(OBJ) -o $(TARGET) $(LDFLAGS)

# Rule to compile source files into object files
$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@) # Ensure subdirectories in build/ exist
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -rf $(OBJ_DIR) $(TARGET)

# Phony targets
.PHONY: all clean
