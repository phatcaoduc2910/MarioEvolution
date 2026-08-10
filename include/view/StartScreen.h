#pragma once

#include "view/Screen.h"
#include "core/Types.h"


//Menu đầu game điều hướng bằng phím lên, xuống và Enter.
class StartScreen final : public Screen {
public:
    void render(SDL_Renderer* renderer) const override;
    void setOption(Option option);
    
private:
    Option selectedOption{Option::StartGame};
};
