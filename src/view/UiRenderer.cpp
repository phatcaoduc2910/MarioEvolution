#include "view/UiRenderer.h"

#include <array>
#include <cctype>
#include <unordered_map>

namespace {
constexpr int kGlyphWidth = 5;
constexpr int kGlyphHeight = 7;
constexpr int kGlyphAdvance = 6;

/**
 * Một glyph gồm bảy hàng 5 pixel; bit 1 là pixel cần được vẽ.
 */
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
    {'Y', {17, 17, 10, 4, 4, 4, 4}},     {'Z', {31, 1, 2, 4, 8, 16, 31}}
};

/**
 * Tìm glyph không phân biệt chữ hoa và chữ thường.
 *
 * @param rawCharacter Ký tự cần tìm.
 * @return Glyph tương ứng, hoặc glyph rỗng nếu font không hỗ trợ ký tự.
 */
const Glyph& findGlyph(char rawCharacter) {
    const char character = static_cast<char>(
        std::toupper(static_cast<unsigned char>(rawCharacter))
    );
    const auto found = kFont.find(character);
    return found != kFont.end() ? found->second : kBlankGlyph;
}

/**
 * Tính chiều rộng của một chuỗi bitmap khi render.
 *
 * @param text Chuỗi cần đo.
 * @param scale Hệ số phóng mỗi pixel.
 * @return Chiều rộng chuỗi theo pixel màn hình.
 */
int textWidth(std::string_view text, int scale) {
    return static_cast<int>(text.size()) * kGlyphAdvance * scale;
}
}

/**
 * Tô kín một hình chữ nhật bằng màu RGBA đã cho.
 *
 * @param renderer SDL renderer nhận lệnh vẽ.
 * @param rect Hình chữ nhật đích.
 * @param color Màu cần tô.
 */
void UiRenderer::fillRect(SDL_Renderer* renderer, const SDL_Rect& rect,
                          SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rect);
}

/**
 * Vẽ chuỗi bằng font bitmap 5x7 tích hợp.
 *
 * @param renderer SDL renderer nhận lệnh vẽ.
 * @param text Chuỗi cần vẽ.
 * @param x Tọa độ x bắt đầu.
 * @param y Tọa độ y bắt đầu.
 * @param scale Hệ số phóng mỗi pixel.
 * @param color Màu chữ.
 */
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

/**
 * Vẽ chuỗi bitmap căn giữa theo chiều rộng màn hình.
 *
 * @param renderer SDL renderer nhận lệnh vẽ.
 * @param text Chuỗi cần vẽ.
 * @param screenWidth Chiều rộng màn hình dùng để căn giữa.
 * @param y Tọa độ y bắt đầu.
 * @param scale Hệ số phóng mỗi pixel.
 * @param color Màu chữ.
 */
void UiRenderer::drawCenteredText(SDL_Renderer* renderer,
                                  std::string_view text, int screenWidth,
                                  int y, int scale, SDL_Color color) {
    const int x = (screenWidth - textWidth(text, scale)) / 2;
    drawText(renderer, text, x, y, scale, color);
}
