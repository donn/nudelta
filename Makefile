SOURCES = $(shell find src | grep .cpp)
HEADERS = $(shell find include | grep .hpp)
OBJECTS = $(addprefix build/,$(patsubst %.cpp,%.o,$(SOURCES)))
BINARY = nudelta

$(BINARY): $(OBJECTS)
	c++ -o $@ -lhidapi-libusb -lfmt $<

$(OBJECTS): build/%.o : %.cpp $(HEADERS)
	@mkdir -p $(@D)
	c++ -g -c -std=c++17 -Iinclude -o $@ $<

