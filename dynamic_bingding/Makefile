SRC := ./polymorphic.cpp
TARGET := ./polymorphic

CXX := g++

CXXFLAGS := -std=c++17

$(TARGET) : $(SRC)
	@$(CXX) $(CXXFLAGS) $< -o $@ 

.PHONY : clean

clean : 
	@rm -rf $(TARGET)