# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++11

# Source file
SRC = assign_1.cpp

# Object file
OBJ = mylinker

# Target rule to create the object file
all: $(OBJ)

# Rule to create the executable from the source file
$(OBJ): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(OBJ) $(SRC)

# Clean rule to remove the object file
clean:
	rm -f $(OBJ)

