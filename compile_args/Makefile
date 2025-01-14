SRC := ./cav1.cpp
TARGET := ./cav1

CXX := g++

CXXFLAGS := -std=c++17

$(TARGET) : $(SRC)
	@$(CXX) $(CXXFLAGS) $< -o $@ 

.PHONY : clean

clean : 
	@rm -rf $(TARGET)