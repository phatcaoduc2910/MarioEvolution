SHELL := C:/msys64/usr/bin/sh.exe

MSYS2_PREFIX ?= /ucrt64
export PATH := $(MSYS2_PREFIX)/bin:/usr/bin:$(PATH)
CXX = $(MSYS2_PREFIX)/bin/g++

PKG_CONFIG = $(MSYS2_PREFIX)/bin/pkg-config
SDL_IMAGE_LIBS = $(filter-out -mwindows -lSDL2main,$(shell $(PKG_CONFIG) --static --libs SDL2_image))

CXXFLAGS = -Wall -Wextra -std=c++17 -Iinclude -I$(MSYS2_PREFIX)/include -DSDL_MAIN_HANDLED
LDFLAGS = -static
LDLIBS = $(SDL_IMAGE_LIBS)

APP = MarioEvolution.exe
SRC = $(wildcard src/*.cpp src/*/*.cpp)
OBJDIR = builds
OBJ = $(SRC:src/%.cpp=$(OBJDIR)/%.o)

RENDER_PREVIEW_APP = $(OBJDIR)/render_preview/RenderPreview.exe
RENDER_PREVIEW_SRC = tools/render_preview/main.cpp \
	src/view/SpriteAnimation.cpp

LEVEL_CODEC_TEST_APP = $(OBJDIR)/tests/LevelCodecTest.exe
LEVEL_CODEC_TEST_SRC = tools/level_codec_test/main.cpp \
	src/service/LevelCodec.cpp \
	src/model/LevelData.cpp

TILE_CATALOG_TEST_APP = $(OBJDIR)/tests/TileCatalogTest.exe
TILE_CATALOG_TEST_SRC = tools/tile_catalog_test/main.cpp \
	src/view/TileCatalog.cpp

.PHONY: all create run tools map-editor run-map-editor \
	render-preview run-render-preview test test-level-codec \
	test-tile-catalog clean

all: create

create: $(APP)
$(APP): $(OBJ)
	$(CXX) $(OBJ) $(LDFLAGS) $(LDLIBS) -o $(APP)

$(OBJDIR)/%.o: src/%.cpp
	mkdir -p "$(@D)"
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: create
	@echo "Đang khởi động game..."
	PATH="$(MSYS2_PREFIX)/bin:$$PATH" ./$(APP)

tools: render-preview

map-editor: create
	@echo "Map editor is embedded in the game; press 0 while playing."

run-map-editor: create
	@echo "Starting the game; press 0 while playing to open the editor."
	PATH="$(MSYS2_PREFIX)/bin:$$PATH" ./$(APP)

render-preview: $(RENDER_PREVIEW_APP)
$(RENDER_PREVIEW_APP): $(RENDER_PREVIEW_SRC)
	mkdir -p "$(@D)"
	$(CXX) $(CXXFLAGS) $(RENDER_PREVIEW_SRC) $(LDFLAGS) $(LDLIBS) -o $@

run-render-preview: render-preview
	PATH="$(MSYS2_PREFIX)/bin:$$PATH" "$(RENDER_PREVIEW_APP)"

test: test-level-codec test-tile-catalog

test-level-codec: $(LEVEL_CODEC_TEST_APP)
	./$(LEVEL_CODEC_TEST_APP)

$(LEVEL_CODEC_TEST_APP): $(LEVEL_CODEC_TEST_SRC) assets/level1.map
	mkdir -p "$(@D)"
	$(CXX) $(CXXFLAGS) $(LEVEL_CODEC_TEST_SRC) $(LDFLAGS) -o $@

test-tile-catalog: $(TILE_CATALOG_TEST_APP)
	./$(TILE_CATALOG_TEST_APP)

$(TILE_CATALOG_TEST_APP): $(TILE_CATALOG_TEST_SRC)
	mkdir -p "$(@D)"
	$(CXX) $(CXXFLAGS) $(TILE_CATALOG_TEST_SRC) $(LDFLAGS) -o $@

clean:
	@echo "Đang dọn dep..."
	rm -f "$(APP)"
	rm -rf "$(OBJDIR)"
