SRC := ./tmpv2.cpp
TARGET := ./tmpv2

CXX := g++

CXXFLAGS := -std=c++17

$(TARGET) : $(SRC)
	@$(CXX) $(CXXFLAGS) $< -o $@ 

.PHONY : clean

clean : 
	@rm -rf $(TARGET)