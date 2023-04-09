CXX        = g++
CXXFLAGS   = -mavx2 -msse3
SOURCES    = Alpga_Blending.cpp
SFML	   = -lsfml-graphics -lsfml-window -lsfml-system
OBJECTS    = $(SOURCES:.cpp=.o)
EXECUTABLE = blnd

.PHONY: compile
compile: $(SOURCES) $(EXECUTABLE)
				./blnd

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(SFML)

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $< -o $@

.PHONY: run
run:
	./blnd

.PHONY: clear
clear:
	rm -rf $(OBJECTS) $(EXECUTABLE)

clrObj:
	rm -rf $(OBJECTS)
