#pragma once

class InputHandler;

class Screen {
public:
    virtual ~Screen() = default;

    virtual void render();
    virtual void handleInput(InputHandler& input);
};

class StartScreen : public Screen {
public:
    StartScreen();

    void displayMenu();
    void render() override;
    void handleInput(InputHandler& input) override;

private:
    int selectedOption;
};

class PauseScreen : public Screen {
public:
    PauseScreen();

    bool isPaused() const;
    void display();
    void render() override;

private:
    bool paused;
};
