#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <string>
#include <unordered_map>

class TextureManager {
public:
    explicit TextureManager(SDL_Renderer* renderer)
        : renderer(renderer) {}
    ~TextureManager();
    bool backgroundTextureLoad();
    bool playerTextureLoad();
    bool effectTextureLoad();
    bool enemyTextureLoad();
    bool itemTextureLoad();
    bool tileTextureLoad();
    bool uiTextureLoad();
    bool gameTextureLoad();
    
    SDL_Texture* getTexture(const std::string& id) const;
    void clean();
private:
    SDL_Renderer* renderer;
    std::unordered_map<std::string, SDL_Texture*> textureMap;
    bool preload(const std::string& id, const std::string& filePath);
};