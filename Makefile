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

MAP_EDITOR_APP = $(OBJDIR)/map_editor/MapEditor.exe
MAP_EDITOR_SRC = tools/map_editor/main.cpp \
	src/service/MapEditorService.cpp \
	src/service/LevelCodec.cpp \
	src/model/LevelData.cpp

RENDER_PREVIEW_APP = $(OBJDIR)/render_preview/RenderPreview.exe
RENDER_PREVIEW_SRC = tools/render_preview/main.cpp \
	src/view/SpriteAnimation.cpp

LEVEL_CODEC_TEST_APP = $(OBJDIR)/tests/LevelCodecTest.exe
LEVEL_CODEC_TEST_SRC = tools/level_codec_test/main.cpp \
	src/service/LevelCodec.cpp \
	src/model/LevelData.cpp

.PHONY: all create run tools map-editor run-map-editor \
	render-preview run-render-preview test test-level-codec clean

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

tools: map-editor render-preview

map-editor: $(MAP_EDITOR_APP)
$(MAP_EDITOR_APP): $(MAP_EDITOR_SRC)
	mkdir -p "$(@D)"
	$(CXX) $(CXXFLAGS) $(MAP_EDITOR_SRC) $(LDFLAGS) $(LDLIBS) -o $@

run-map-editor: map-editor
	PATH="$(MSYS2_PREFIX)/bin:$$PATH" "$(MAP_EDITOR_APP)"

render-preview: $(RENDER_PREVIEW_APP)
$(RENDER_PREVIEW_APP): $(RENDER_PREVIEW_SRC)
	mkdir -p "$(@D)"
	$(CXX) $(CXXFLAGS) $(RENDER_PREVIEW_SRC) $(LDFLAGS) $(LDLIBS) -o $@

run-render-preview: render-preview
	PATH="$(MSYS2_PREFIX)/bin:$$PATH" "$(RENDER_PREVIEW_APP)"

test: test-level-codec

test-level-codec: $(LEVEL_CODEC_TEST_APP)
	./$(LEVEL_CODEC_TEST_APP)

$(LEVEL_CODEC_TEST_APP): $(LEVEL_CODEC_TEST_SRC) assets/level1.map
	mkdir -p "$(@D)"
	$(CXX) $(CXXFLAGS) $(LEVEL_CODEC_TEST_SRC) $(LDFLAGS) -o $@

clean:
	@echo "Đang dọn dep..."
	rm -f "$(APP)"
	rm -rf "$(OBJDIR)"
