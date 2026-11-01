CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra

TARGET = tividy

# Every .cpp in the project, except unit tests and CMake build folders
# (each pattern's tests are built with its own CMakeLists + GoogleTest).
SOURCES = $(shell find . -name "*.cpp" -not -path "*/tests/*" -not -path "*/build/*")

# Every folder that holds a header, so #include "X.h" works from anywhere
INCLUDES = $(addprefix -I,$(sort $(dir $(shell find . -name "*.h" -not -path "*/tests/*" -not -path "*/build/*"))))

OBJECTS = $(SOURCES:.cpp=.o)
DEPS    = $(OBJECTS:.o=.d)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -MMD -MP -c $< -o $@   # -MMD: rebuild when a header changes

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJECTS) $(DEPS) $(TARGET)

-include $(DEPS)

.PHONY: all run clean
