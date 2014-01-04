# Makefile

ifeq ($(RELEASE),1)
	CXXFLAGS += -O3
else
	CXXFLAGS += -g -O0
endif

CXXFLAGS += -pipe -I$(BOOST_ROOT)\\include -Ilibpng\\include -Izlib\\include -Icapdll -D_WIN32_IE=0x0300 -std=c++11 -Wall -Wextra -pedantic-errors
LDFLAGS += -Llibpng\\lib -mwindows

MAIN_DIRS = capture capture/util capture/util/windows
MAIN_SOURCES = $(wildcard $(addsuffix /*.cpp,$(MAIN_DIRS)))
MAIN_OBJECTS = $(MAIN_SOURCES:.cpp=.o) capture/resources.o
MAIN_LIBS = -lgdi32 -lcomctl32 -lcomdlg32 -lpng

DLL_SOURCES = $(wildcard capdll/*.cpp)
DLL_OBJECTS = $(DLL_SOURCES:.cpp=.o)

.PHONY: all
all: capture.exe

.PHONY: clean
clean:
	rm -f $(MAIN_OBJECTS) $(DLL_OBJECTS)
	rm -f capture.exe capdll.dll capdll.a

capture.exe: $(MAIN_OBJECTS) capdll.a
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(MAIN_LIBS)
ifeq ($(RELEASE),1)
	strip $@
endif

capture/resources.o: capture/main.rc
	windres $< $@

capdll.dll: $(DLL_OBJECTS) capdll/main.def
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -shared -o $@ $^ $(DLL_LIBS) -Wl,--enable-stdcall-fixup
ifeq ($(RELEASE),1)
	strip $@
endif

capdll.a: capdll.dll
	dlltool -l $@ --dllname $< --def capdll/main.def

capdll/%.o: capdll/%.cpp
	$(CXX) $(CXXFLAGS) -DDLL_EXPORT -c -o $@ $<

.PHONY: depend
depend:
	rm -f Makefile.depend
	$(CXX) $(CXXFLAGS) -MM $(MAIN_SOURCES) | sed -e 's/\([^:]\+\):/capture\/\1:/' >> Makefile.depend
	$(CXX) $(CXXFLAGS) -MM $(DLL_SOURCES) | sed -e 's/\([^:]\+\):/capdll\/\1:/' >> Makefile.depend
-include Makefile.depend
