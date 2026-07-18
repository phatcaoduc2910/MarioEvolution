CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -Iinclude -DSDL_MAIN_HANDLED
LDFLAGS =
LDLIBS = -lSDL2 -lSDL2_image

APP = MarioEvolution
SRC = $(wildcard src/*.cpp src/*/*.cpp)
OBJDIR = builds
OBJ = $(SRC:src/%.cpp=$(OBJDIR)/%.o)

.PHONY: all create run clean

all: create

create: $(APP)
$(APP): $(OBJ)
	$(CXX) $(OBJ) $(LDFLAGS) $(LDLIBS) -o $(APP)

$(OBJDIR)/%.o: src/%.cpp
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: create
	@echo "Đang khởi động game..."
	./$(APP)

clean:
	@echo "Đang dọn dep..."
	rm -f $(APP)
	rm -rf $(OBJDIR)
