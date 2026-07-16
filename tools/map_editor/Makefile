CXX = g++

ROOT = ../..
SDL2_ROOT ?= C:/SDL2/SDL2-2.32.10/x86_64-w64-mingw32

CXXFLAGS = -Wall -Wextra -std=c++17 -I$(ROOT)/include -I$(SDL2_ROOT)/include -DSDL_MAIN_HANDLED
LDFLAGS = -L$(SDL2_ROOT)/lib
LDLIBS = -lSDL2

APP = $(ROOT)/builds/map_editor/MapEditor.exe
SRC = main.cpp $(ROOT)/src/service/MapEditorService.cpp $(ROOT)/src/model/LevelData.cpp

.PHONY: all create run clean

all: create

create:
	@if not exist "$(ROOT)\builds\map_editor" mkdir "$(ROOT)\builds\map_editor"
	$(CXX) $(CXXFLAGS) $(SRC) $(LDFLAGS) $(LDLIBS) -o $(APP)

run: create
	@.\..\..\builds\map_editor\MapEditor.exe

clean:
	@if exist "$(ROOT)\builds\map_editor\MapEditor.exe" del /Q "$(ROOT)\builds\map_editor\MapEditor.exe"
