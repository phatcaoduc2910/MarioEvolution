#pragma once

#include <SDL2/SDL.h>

class InputHandler;

class Screen {
public:
    virtual ~Screen() = default;

    virtual void render(SDL_Renderer* renderer);
    virtual void handleInput(InputHandler& input);
};

class StartScreen : public Screen {
public:
    StartScreen();

    int getSelectedOption() const;
    void displayMenu(SDL_Renderer* renderer);
    void render(SDL_Renderer* renderer) override;
    void handleInput(InputHandler& input) override;

private:
    int selectedOption;
};

class PauseScreen : public Screen {
public:
    PauseScreen();

    bool isPaused() const;
    void display(SDL_Renderer* renderer);
    void render(SDL_Renderer* renderer) override;
    void handleInput(InputHandler& input) override;

private:
    bool paused;
};
