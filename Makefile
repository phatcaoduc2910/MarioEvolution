SHELL := C:/msys64/usr/bin/sh.exe

MSYS2_PREFIX ?= /ucrt64
export PATH := $(MSYS2_PREFIX)/bin:/usr/bin:$(PATH)
CXX = $(MSYS2_PREFIX)/bin/g++

PKG_CONFIG = $(MSYS2_PREFIX)/bin/pkg-config
SDL_IMAGE_LIBS = $(filter-out -mwindows -lSDL2main,$(shell $(PKG_CONFIG) --static --libs SDL2_image))
SDL_MIXER_LIBS = $(filter-out -mwindows -lSDL2main,$(shell $(PKG_CONFIG) --static --libs SDL2_mixer))

CXXFLAGS = -Wall -Wextra -std=c++17 -MMD -MP -Iinclude -I$(MSYS2_PREFIX)/include -DSDL_MAIN_HANDLED
LDFLAGS = -static
LDLIBS = $(SDL_IMAGE_LIBS)
APP_LDLIBS = $(LDLIBS) $(SDL_MIXER_LIBS)

APP = MarioEvolution.exe
SRC = $(wildcard src/*.cpp src/*/*.cpp)
OBJDIR = builds
OBJ = $(SRC:src/%.cpp=$(OBJDIR)/%.o)
DEP = $(OBJ:.o=.d)

LEVEL_CODEC_TEST_APP = $(OBJDIR)/tests/LevelCodecTest.exe
LEVEL_CODEC_TEST_SRC = tests/level_codec_test.cpp \
	src/service/LevelCodec.cpp \
	src/model/LevelData.cpp

TILE_CATALOG_TEST_APP = $(OBJDIR)/tests/TileCatalogTest.exe
TILE_CATALOG_TEST_SRC = tests/tile_catalog_test.cpp \
	src/view/TileCatalog.cpp

MAP_EDITOR_TEST_APP = $(OBJDIR)/tests/MapEditorTest.exe
MAP_EDITOR_TEST_SRC = tests/map_editor_test.cpp \
	src/service/MapEditorService.cpp \
	src/service/LevelCodec.cpp \
	src/model/LevelData.cpp \
	src/view/TileCatalog.cpp \
	src/view/TextureManager.cpp \
	src/view/UiRenderer.cpp

.PHONY: all create run map-editor run-map-editor \
	test test-level-codec \
	test-tile-catalog test-map-editor clean

all: create

create: $(APP)
$(APP): $(OBJ)
	$(CXX) $(OBJ) $(LDFLAGS) $(APP_LDLIBS) -o $(APP)

$(OBJDIR)/%.o: src/%.cpp
	mkdir -p "$(@D)"
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: create
	@echo "Đang khởi động game..."
	PATH="$(MSYS2_PREFIX)/bin:$$PATH" ./$(APP)

map-editor: create
	@echo "Map editor is embedded in the game; press 0 while playing."

run-map-editor: create
	@echo "Starting the game; press 0 while playing to open the editor."
	PATH="$(MSYS2_PREFIX)/bin:$$PATH" ./$(APP)

test: test-level-codec test-tile-catalog test-map-editor

test-level-codec: $(LEVEL_CODEC_TEST_APP)
	./$(LEVEL_CODEC_TEST_APP)

$(LEVEL_CODEC_TEST_APP): $(LEVEL_CODEC_TEST_SRC) assets/maps/level1.map
	mkdir -p "$(@D)"
	$(CXX) $(CXXFLAGS) $(LEVEL_CODEC_TEST_SRC) $(LDFLAGS) -o $@

test-tile-catalog: $(TILE_CATALOG_TEST_APP)
	./$(TILE_CATALOG_TEST_APP)

$(TILE_CATALOG_TEST_APP): $(TILE_CATALOG_TEST_SRC)
	mkdir -p "$(@D)"
	$(CXX) $(CXXFLAGS) $(TILE_CATALOG_TEST_SRC) $(LDFLAGS) -o $@

test-map-editor: $(MAP_EDITOR_TEST_APP)
	./$(MAP_EDITOR_TEST_APP)

$(MAP_EDITOR_TEST_APP): $(MAP_EDITOR_TEST_SRC)
	mkdir -p "$(@D)"
	$(CXX) $(CXXFLAGS) $(MAP_EDITOR_TEST_SRC) $(LDFLAGS) $(LDLIBS) -o $@

clean:
	@echo "Đang dọn dep..."
	rm -f "$(APP)"
	rm -rf "$(OBJDIR)"

-include $(DEP)
