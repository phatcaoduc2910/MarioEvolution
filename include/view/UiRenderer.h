#pragma once

#include <SDL2/SDL.h>

#include <string_view>

/**
 * Các thao tác vẽ hình và bitmap text dùng chung giữa các screen.
 */
namespace UiRenderer {
void fillRect(SDL_Renderer* renderer, const SDL_Rect& rect, SDL_Color color);

void drawText(SDL_Renderer* renderer, std::string_view text, int x, int y,
              int scale, SDL_Color color);

void drawCenteredText(SDL_Renderer* renderer, std::string_view text,
                      int screenWidth, int y, int scale, SDL_Color color);
}
