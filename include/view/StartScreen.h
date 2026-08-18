#pragma once

#include "view/Screen.h"

#include <string_view>

enum class StartMenuAction {
    None,
    StartGame,
    MapEditor,
    ExitGame
};

// Menu đầu game điều hướng bằng phím lên, xuống và Enter.
class StartScreen final : public Screen {
public:
    void render(SDL_Renderer* renderer) const override;
    void render(SDL_Renderer* renderer, std::string_view levelName) const;
    void moveSelection(int direction);
    StartMenuAction getSelectedAction() const;
    StartMenuAction actionAt(SDL_Renderer* renderer, int x, int y) const;
    int levelDirectionAt(SDL_Renderer* renderer, int x, int y) const;

private:
    StartMenuAction selectedAction{StartMenuAction::StartGame};
};
