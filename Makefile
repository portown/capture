# Makefile

ifeq ($(RELEASE),1)
	CXXFLAGS += -O3
	LDFLAGS += -mwindows
else
	CXXFLAGS += -g -O0
endif

CXXFLAGS += -pipe -std=c++17 -Wall -Wextra -pedantic-errors
CXXFLAGS += -I$(LIBPNG_ROOT)/include -I$(BOOST_ROOT)/include -Icapdll -D_WIN32_IE=0x0300
LDFLAGS += -L$(LIBPNG_ROOT)/lib -L$(BOOST_ROOT)/lib -static-libgcc -static-libstdc++

MAIN_BIN = capture.exe
MAIN_DIRS = src src/view
MAIN_SOURCES = $(wildcard $(addsuffix /*.cpp,$(MAIN_DIRS)))
MAIN_OBJECTS = $(MAIN_SOURCES:.cpp=.o)
MAIN_DEPENDS = $(MAIN_SOURCES:.cpp=.d)
MAIN_LIBS = -lgdi32 -lcomctl32 -lcomdlg32 -lpng -lz -lboost_system -lboost_container -lboost_filesystem

MAIN_RESOURCES = src/resources.o
MAIN_RCS = src/main.rc
MAIN_RC_DEPENDS = src/resource.h

DLL_BIN = capdll.dll
DLL_LIB = lib$(DLL_BIN).a
DLL_SOURCES = $(wildcard capdll/*.cpp)
DLL_OBJECTS = $(DLL_SOURCES:.cpp=.o)
DLL_DEPENDS = $(DLL_SOURCES:.cpp=.d)

.PHONY: all
all: $(MAIN_BIN)

.PHONY: clean
clean:
	rm -f $(MAIN_OBJECTS) $(MAIN_RESOURCES) $(DLL_OBJECTS)
	rm -f $(MAIN_DEPENDS) $(DLL_DEPENDS)
	rm -f $(MAIN_BIN) $(DLL_BIN) $(DLL_LIB)

$(MAIN_BIN): $(MAIN_OBJECTS) $(MAIN_RESOURCES) $(DLL_LIB)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(MAIN_LIBS)
ifeq ($(RELEASE),1)
	$(STRIP) $@
endif

$(MAIN_RESOURCES): $(MAIN_RCS) $(MAIN_RC_DEPENDS)
	$(WINDRES) $< $@

$(DLL_BIN): $(DLL_OBJECTS) capdll/main.def
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -shared -o $@ $^ $(DLL_LIBS) -Wl,--enable-stdcall-fixup
ifeq ($(RELEASE),1)
	$(STRIP) $@
endif

$(DLL_LIB): $(DLL_BIN)
	$(DLLTOOL) -l $@ --dllname $< --def capdll/main.def

capdll/%.o: capdll/%.cpp
	$(CXX) $(CXXFLAGS) -DDLL_EXPORT -c -o $@ $<


%.d: %.cpp
	$(CXX) $(CXXFLAGS) -MM -MF $@ -MT '$@ $(patsubst %.cpp,%.o,$<)' $<
-include $(MAIN_DEPENDS) $(DLL_DEPENDS)
