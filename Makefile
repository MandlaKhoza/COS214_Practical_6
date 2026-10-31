CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra

TARGET = tividy

SOURCES = $(shell find . -name "*.cpp")

OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run:
	./tividy

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean
