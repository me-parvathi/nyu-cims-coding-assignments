# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++11

# Source file
SRC = assign_2.cpp

# Object file
OBJ = myschedulerrr

# Target rule to create the object file
all: $(OBJ)

# Rule to create the executable from the source file
$(OBJ): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(OBJ) $(SRC)

# Clean rule to remove the object file
clean:
	rm -f $(OBJ)