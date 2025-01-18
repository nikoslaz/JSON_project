CXX = g++
CXXFLAGS = -Wall -Wextra -Iinclude
SRC = src/main.cpp
OUT = JSON

all: $(OUT)

$(OUT): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(OUT) $(SRC)

clean:
	rm -f $(OUT)

.PHONY: all clean
