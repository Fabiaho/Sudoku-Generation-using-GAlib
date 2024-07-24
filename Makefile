# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -O3 -fpermissive -I/usr/local/include/galib -I./database
LDFLAGS = -L/usr/local/lib -lga -lmysqlcppconn

# Source files
SOURCES = sudoku.cpp database/db_connector.cpp
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = sudoku

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
