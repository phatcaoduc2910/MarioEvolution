#include "view/EffectManager.h"

#include <algorithm>
#include <utility>

namespace {
constexpr int kSmokeFrameCount = 2;
constexpr int kImpactFrameDurationMs = 60;
constexpr int kImpactLifetimeMs = 120;
constexpr double kImpactAnchorOffsetPixels = 8.0;
constexpr std::size_t kMaxActiveEffects = 48;

struct SmokePuff {
    double offsetX;
    double offsetY;
    double scale;
    int lifetimeMs;
};

constexpr SmokePuff kSmokePuffs[] = {
    {-8.0,  0.0, 1.0, 260},
    { 1.0, -5.0, 0.8, 200},
    { 8.0, -1.0, 1.2, 320},
};

Uint8 fadeAlpha(int elapsedMs, int lifetimeMs) {
    if (lifetimeMs <= 0) {
        return SDL_ALPHA_OPAQUE;
    }

    const double remaining =
        1.0 - static_cast<double>(elapsedMs) / static_cast<double>(lifetimeMs);
    return static_cast<Uint8>(
        std::clamp(remaining, 0.0, 1.0) * SDL_ALPHA_OPAQUE);
}
}

void EffectManager::spawnSmoke(double centerX, double bottomY) {
    for (const SmokePuff& puff : kSmokePuffs) {
        spawn({SpriteAnimation({"smoke.1", "smoke.2"},
                               puff.lifetimeMs / kSmokeFrameCount, false),
               centerX + puff.offsetX,
               bottomY + puff.offsetY,
               puff.scale,
               0,
               puff.lifetimeMs});
    }
}

void EffectManager::spawnImpact(double centerX, double centerY) {
    spawn({SpriteAnimation({"smoke.2", "smoke.1"}, kImpactFrameDurationMs, false),
           centerX,
           centerY + kImpactAnchorOffsetPixels,
           1.0,
           0,
           kImpactLifetimeMs});
}

void EffectManager::spawn(Effect effect) {
    if (effects.size() >= kMaxActiveEffects) {
        return;
    }

    effects.push_back(std::move(effect));
}

void EffectManager::update(int deltaMs) {
    for (Effect& effect : effects) {
        effect.elapsedMs += deltaMs;
        effect.animation.update(deltaMs);
    }

    effects.erase(
        std::remove_if(
            effects.begin(),
            effects.end(),
            [](const Effect& effect) {
                return effect.elapsedMs >= effect.lifetimeMs;
            }),
        effects.end());
}

void EffectManager::render(SDL_Renderer* renderer,
                           const TextureManager& textures,
                           int offsetX,
                           int offsetY) {
    if (renderer == nullptr) {
        return;
    }

    for (const Effect& effect : effects) {
        SDL_Texture* texture =
            textures.getTexture(effect.animation.getCurrentFrameId());
        if (texture == nullptr) {
            continue;
        }

        int frameWidth = 0;
        int frameHeight = 0;
        if (SDL_QueryTexture(
                texture, nullptr, nullptr, &frameWidth, &frameHeight) < 0) {
            continue;
        }

        const int renderWidth =
            static_cast<int>(frameWidth * effect.scale);
        const int renderHeight =
            static_cast<int>(frameHeight * effect.scale);
        if (renderWidth <= 0 || renderHeight <= 0) {
            continue;
        }

        const SDL_Rect destination{
            static_cast<int>(effect.centerX) - renderWidth / 2 + offsetX,
            static_cast<int>(effect.bottomY) - renderHeight + offsetY,
            renderWidth,
            renderHeight
        };

        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
        SDL_SetTextureAlphaMod(
            texture, fadeAlpha(effect.elapsedMs, effect.lifetimeMs));
        assetRenderer.render(renderer, texture, nullptr, &destination);
        SDL_SetTextureAlphaMod(texture, SDL_ALPHA_OPAQUE);
    }
}

void EffectManager::clear() {
    effects.clear();
}

std::size_t EffectManager::activeCount() const {
    return effects.size();
}
