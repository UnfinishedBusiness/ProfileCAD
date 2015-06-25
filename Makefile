appname := ProfileCAD

CXX := g++
CXXFLAGS := -std=c++11 -I./inc/ $(shell pkg-config --cflags glib-2.0)
LDFLAGS := -lSDL2 -lSDL2_image -lSDL2_ttf $(shell pkg-config --libs glib-2.0)
srcfiles := $(shell find ./src/ -name "*.cpp")
objects  := $(patsubst %.cpp, %.o, $(srcfiles))

all: $(appname)

$(appname): $(objects)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(appname) $(objects) $(LDLIBS)

depend: .depend

.depend: $(srcfiles)
	rm -f ./.depend
	$(CXX) $(CXXFLAGS) -MM $^>>./.depend;

clean:
	rm -f $(objects)
	rm -f $(appname)

dist-clean: clean
	rm -f *~ .depend

include .depend
