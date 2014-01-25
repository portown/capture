# Makefile

ifeq ($(RELEASE),1)
	CXXFLAGS += -O3
	LDFLAGS += -mwindows -static-libgcc -static-libstdc++
else
	CXXFLAGS += -g -O0
endif

CXXFLAGS += -pipe -std=c++11 -Wall -Wextra -pedantic-errors
CXXFLAGS += -I$(BOOST_ROOT)\include -Icapdll -D_WIN32_IE=0x0300

MAIN_BIN = capture.exe
MAIN_DIRS = capture capture/util capture/util/windows
MAIN_SOURCES = $(wildcard $(addsuffix /*.cpp,$(MAIN_DIRS)))
MAIN_OBJECTS = $(MAIN_SOURCES:.cpp=.o) capture/resources.o
MAIN_DEPENDS = $(MAIN_SOURCES:.cpp=.d)
MAIN_LIBS = -lgdi32 -lcomctl32 -lcomdlg32 -lpng -lz

DLL_BIN = capdll.dll
DLL_LIB = lib$(DLL_BIN).a
DLL_SOURCES = $(wildcard capdll/*.cpp)
DLL_OBJECTS = $(DLL_SOURCES:.cpp=.o)
DLL_DEPENDS = $(DLL_SOURCES:.cpp=.d)

.PHONY: all
all: $(MAIN_BIN)

.PHONY: clean
clean:
	rm -f $(MAIN_OBJECTS) $(DLL_OBJECTS)
	rm -f $(MAIN_BIN) $(DLL_BIN) $(DLL_LIB)

capture.exe: $(MAIN_OBJECTS) $(DLL_LIB)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(MAIN_LIBS)
ifeq ($(RELEASE),1)
	strip $@
endif

capture/resources.o: capture/main.rc capture/resource.h
	windres $< $@

$(DLL_BIN): $(DLL_OBJECTS) capdll/main.def
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -shared -o $@ $^ $(DLL_LIBS) -Wl,--enable-stdcall-fixup
ifeq ($(RELEASE),1)
	strip $@
endif

$(DLL_LIB): $(DLL_BIN)
	dlltool -l $@ --dllname $< --def capdll/main.def

capdll/%.o: capdll/%.cpp
	$(CXX) $(CXXFLAGS) -DDLL_EXPORT -c -o $@ $<


%.d: %.cpp
	$(CXX) $(CXXFLAGS) -MM $< > $@
	sed -i -e 's/\(^\S[^:]*\):/$(subst /,\/,$(dir $@))\1 $(subst /,\/,$@):/' $@
-include $(MAIN_DEPENDS) $(DLL_DEPENDS)
