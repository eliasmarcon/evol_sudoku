CC = g++
CFLAGS = -std=c++11
LIBS = -lga

TARGET = sudoku_genetic

all: $(TARGET)

$(TARGET): sudoku_genetic.cpp
	$(CC) $(CFLAGS) -o $(TARGET) sudoku_genetic.cpp $(LIBS) -fpermissive -Ofast

.PHONY: clean

clean:
	rm -f $(TARGET)