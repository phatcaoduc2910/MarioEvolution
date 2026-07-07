CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17
LDFLAGS =
LDLIBS = -lSDL2

APP = MarioEvolution
SRC = src/main.cpp
OBJDIR = builds
OBJ = $(SRC:src/%.cpp=$(OBJDIR)/%.o)

.PHONY: create run clean 

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