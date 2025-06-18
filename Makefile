# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Iheaders -fopenmp -Wall -Wextra

# Source files
SRCS = main.cpp utils.cpp memetic_core.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Executable name
TARGET = MC_memetic.out

# Default target
all: $(TARGET)
	@echo "Build finished, cleaning up object files..."
	@rm -f $(OBJS)

# Link objects into executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile .cpp to .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: all clean

clean:
	rm -f $(OBJS) $(TARGET)