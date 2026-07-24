#include "view/UiRenderer.h"

#include <array>
#include <cctype>
#include <unordered_map>

namespace {
constexpr int kGlyphWidth = 5;
constexpr int kGlyphHeight = 7;
constexpr int kGlyphAdvance = 6;

// Mỗi byte là một hàng; năm bit thấp quyết định pixel nào được bật.
using Glyph = std::array<unsigned char, kGlyphHeight>;

const Glyph kBlankGlyph{};
const std::unordered_map<char, Glyph> kFont = {
    {'A', {14, 17, 17, 31, 17, 17, 17}}, {'B', {30, 17, 17, 30, 17, 17, 30}},
    {'C', {14, 17, 16, 16, 16, 17, 14}}, {'D', {30, 17, 17, 17, 17, 17, 30}},
    {'E', {31, 16, 16, 30, 16, 16, 31}}, {'F', {31, 16, 16, 30, 16, 16, 16}},
    {'G', {14, 17, 16, 23, 17, 17, 15}}, {'H', {17, 17, 17, 31, 17, 17, 17}},
    {'I', {14, 4, 4, 4, 4, 4, 14}},      {'J', {7, 2, 2, 2, 2, 18, 12}},
    {'K', {17, 18, 20, 24, 20, 18, 17}}, {'L', {16, 16, 16, 16, 16, 16, 31}},
    {'M', {17, 27, 21, 21, 17, 17, 17}}, {'N', {17, 25, 21, 19, 17, 17, 17}},
    {'O', {14, 17, 17, 17, 17, 17, 14}}, {'P', {30, 17, 17, 30, 16, 16, 16}},
    {'Q', {14, 17, 17, 17, 21, 18, 13}}, {'R', {30, 17, 17, 30, 20, 18, 17}},
    {'S', {15, 16, 16, 14, 1, 1, 30}},   {'T', {31, 4, 4, 4, 4, 4, 4}},
    {'U', {17, 17, 17, 17, 17, 17, 14}}, {'V', {17, 17, 17, 17, 17, 10, 4}},
    {'W', {17, 17, 17, 21, 21, 21, 10}}, {'X', {17, 17, 10, 4, 10, 17, 17}},
    {'Y', {17, 17, 10, 4, 4, 4, 4}},     {'Z', {31, 1, 2, 4, 8, 16, 31}},
    {'0', {14, 17, 19, 21, 25, 17, 14}}, {'1', {4, 12, 4, 4, 4, 4, 14}},
    {'2', {14, 17, 1, 2, 4, 8, 31}},      {'3', {30, 1, 1, 14, 1, 1, 30}},
    {'4', {2, 6, 10, 18, 31, 2, 2}},      {'5', {31, 16, 16, 30, 1, 1, 30}},
    {'6', {14, 16, 16, 30, 17, 17, 14}},  {'7', {31, 1, 2, 4, 8, 8, 8}},
    {'8', {14, 17, 17, 14, 17, 17, 14}},  {'9', {14, 17, 17, 15, 1, 1, 14}},
    {'-', {0, 0, 0, 31, 0, 0, 0}},        {'_', {0, 0, 0, 0, 0, 0, 31}},
    {'+', {0, 4, 4, 31, 4, 4, 0}}
};

const Glyph& findGlyph(char rawCharacter) {
    const char character = static_cast<char>(
        std::toupper(static_cast<unsigned char>(rawCharacter))
    );
    const auto found = kFont.find(character);
    return found != kFont.end() ? found->second : kBlankGlyph;
}

int textWidth(std::string_view text, int scale) {
    return static_cast<int>(text.size()) * kGlyphAdvance * scale;
}
}

void UiRenderer::fillRect(SDL_Renderer* renderer, const SDL_Rect& rect,
                          SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rect);
}

void UiRenderer::drawText(SDL_Renderer* renderer, std::string_view text,
                          int x, int y, int scale, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    int cursorX = x;

    for (char character : text) {
        const Glyph& glyph = findGlyph(character);

        for (int row = 0; row < kGlyphHeight; ++row) {
            for (int column = 0; column < kGlyphWidth; ++column) {
                const int pixelMask = 1 << (kGlyphWidth - 1 - column);
                if ((glyph[row] & pixelMask) == 0) {
                    continue;
                }

                const SDL_Rect pixel{
                    cursorX + column * scale,
                    y + row * scale,
                    scale,
                    scale
                };
                SDL_RenderFillRect(renderer, &pixel);
            }
        }

        cursorX += kGlyphAdvance * scale;
    }
}

void UiRenderer::drawCenteredText(SDL_Renderer* renderer,
                                  std::string_view text, int screenWidth,
                                  int y, int scale, SDL_Color color) {
    const int x = (screenWidth - textWidth(text, scale)) / 2;
    drawText(renderer, text, x, y, scale, color);
}
