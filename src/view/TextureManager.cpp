#include <view/TextureManager.h>

bool TextureManager::preload(const std::string& id, const std::string& filePath) {
    if (textureMap.find(id) != textureMap.end()) {
        std::cout << "[TextureManager] Key '" << id << "' existed.\n";
        return true;
    }
    SDL_Texture* texture = IMG_LoadTexture(renderer, filePath.c_str());

    if (texture == nullptr) {
        std::cerr << "[TextureManager] Failed to load '" << id
                  << "' from '" << filePath << "': "
                  << IMG_GetError() << '\n';
        return false;
    }

    textureMap.emplace(id, texture);
    return true;
}

bool TextureManager::backgroundTextureLoad() {
    const std::string backgroundPath = "assets/backgrounds/";
    bool success = true;
    success &= preload("background.1", backgroundPath + "level_1/part_1.png");
    success &= preload("background.2", backgroundPath + "level_1/part_2.png");
    success &= preload("sky", backgroundPath + "sky.png");
    return success;
}

bool TextureManager::playerTextureLoad() {
    const std::string playerPath = "assets/characters/mario/";
    bool success = true;
    // Small Mario
    success &= preload("mario.small.walk.1", playerPath + "small/walk_1.png");
    success &= preload("mario.small.walk.2", playerPath + "small/walk_2.png");
    success &= preload("mario.small.walk.3", playerPath + "small/walk_3.png");
    success &= preload("mario.small.jump", playerPath + "small/jump.png");
    success &= preload("mario.small.turn", playerPath + "small/turn.png");
    success &= preload("mario.small.death", playerPath + "small/death.png");
    // Fire Mario
    success &= preload("mario.fire.walk.1", playerPath + "fire/walk_1.png");
    success &= preload("mario.fire.walk.2", playerPath + "fire/walk_2.png");
    success &= preload("mario.fire.walk.3", playerPath + "fire/walk_3.png");
    success &= preload("mario.fire.jump", playerPath + "fire/jump.png");
    success &= preload("mario.fire.turn", playerPath + "fire/turn.png");
    success &= preload("mario.fire.death", playerPath + "fire/death.png");
    success &= preload("mario.fire.crouch", playerPath + "fire/crouch.png");
    success &= preload("mario.fire.slide", playerPath + "fire/slide.png");
    success &= preload("mario.fire.throw.1", playerPath + "fire/throw_1.png");
    success &= preload("mario.fire.throw.2", playerPath + "fire/throw_2.png");
    // Super Mario
    success &= preload("mario.super.walk.1", playerPath + "super/walk_1.png");
    success &= preload("mario.super.walk.2", playerPath + "super/walk_2.png");
    success &= preload("mario.super.walk.3", playerPath + "super/walk_3.png");
    success &= preload("mario.super.walk.4", playerPath + "super/walk_4.png");
    success &= preload("mario.super.jump", playerPath + "super/jump.png");
    success &= preload("mario.super.turn", playerPath + "super/turn.png");
    success &= preload("mario.super.death", playerPath + "super/death.png");
    success &= preload("mario.super.crouch", playerPath + "super/crouch.png");
    success &= preload("mario.super.slide", playerPath + "super/slide.png");
    success &= preload("mario.super.transform", playerPath + "super/transform.png");
    return success;
}

bool TextureManager::effectTextureLoad() {
    const std::string effectPath = "assets/effects/";
    bool success = true;
    success &= preload("flame", effectPath + "projectiles/flame.png");
    success &= preload("smoke.1", effectPath + "smoke/smoke_1.png");
    success &= preload("smoke.2", effectPath + "smoke/smoke_2.png");
    return success;
}

bool TextureManager::enemyTextureLoad() {
    const std::string goombaPath = "assets/enemies/goomba/";
    // Goomba
    bool success = true;
    success &= preload("goomba.walk.1", goombaPath + "walk_1.png");
    success &= preload("goomba.walk.2", goombaPath + "walk_2.png");
    success &= preload("goomba.death", goombaPath + "squished.png");
    // Koopa paratroopa
    const std::string koopaParatroopaPath = "assets/enemies/koopa_paratroopa/";
    success &= preload("koopa.paratroopa.1", koopaParatroopaPath + "frame_1.png");
    success &= preload("koopa.paratroopa.2", koopaParatroopaPath + "frame_2.png");
    success &= preload("koopa.paratroopa.3", koopaParatroopaPath + "frame_3.png");
    success &= preload("koopa.paratroopa.4", koopaParatroopaPath + "frame_4.png");
    // Koopa troopa
    const std::string koopaTroopaPath = "assets/enemies/koopa_";
    // Green Koopa Troopa
    success &= preload("koopa.green.walk.1", koopaTroopaPath + "troopa/green/walk_1.png");
    success &= preload("koopa.green.walk.2", koopaTroopaPath + "troopa/green/walk_2.png");
    success &= preload("koopa.green.shell.1", koopaTroopaPath + "shell/green/frame_1.png");
    success &= preload("koopa.green.shell.2", koopaTroopaPath + "shell/green/frame_2.png");
    success &= preload("koopa.green.shell.3", koopaTroopaPath + "shell/green/frame_3.png");
    success &= preload("koopa.green.shell.4", koopaTroopaPath + "shell/green/frame_4.png");
    // Red Koopa Troopa
    success &= preload("koopa.red.walk.1", koopaTroopaPath + "troopa/red/walk_1.png");
    success &= preload("koopa.red.walk.2", koopaTroopaPath + "troopa/red/walk_2.png");
    success &= preload("koopa.red.shell.1", koopaTroopaPath + "shell/red/frame_1.png");
    success &= preload("koopa.red.shell.2", koopaTroopaPath + "shell/red/frame_2.png");
    success &= preload("koopa.red.shell.3", koopaTroopaPath + "shell/red/frame_3.png");
    success &= preload("koopa.red.shell.4", koopaTroopaPath + "shell/red/frame_4.png");
    // Piranha Plant
    const std::string piranhaPlantPath = "assets/enemies/piranha_plant/";
    success &= preload("piranha.plant.1", piranhaPlantPath + "frame_1.png");
    success &= preload("piranha.plant.2", piranhaPlantPath + "frame_2.png");
    // Venus Fire Trap
    const std::string venusFireTrapPath = "assets/enemies/venus_fire_trap/green/";
    success &= preload("venus.1", venusFireTrapPath + "frame_1.png");
    success &= preload("venus.2", venusFireTrapPath + "frame_2.png");
    success &= preload("venus.3", venusFireTrapPath + "frame_3.png");
    success &= preload("venus.4", venusFireTrapPath + "frame_4.png");
    return success;
}

bool TextureManager::itemTextureLoad() {
    const std::string itemPath = "assets/items/";
    bool success = true;
    // Coin
    success &= preload("coin.1", itemPath + "coins/coin_1.png");
    success &= preload("coin.2", itemPath + "coins/coin_2.png");
    success &= preload("coin.3", itemPath + "coins/coin_3.png");
    success &= preload("coin.4", itemPath + "coins/coin_4.png");
    // Power-ups
    success &= preload("flower", itemPath + "powerups/fire_flower.png");
    success &= preload("star", itemPath + "powerups/star.png");
    return success;
}

bool TextureManager::tileTextureLoad() {
    const std::string tilePath = "assets/tiles/";
    bool success = true;
    // Blocks
    success &= preload("gold", tilePath + "blocks/gold.png");
    success &= preload("question_1", tilePath + "blocks/question_1.png");
    success &= preload("question_2", tilePath + "blocks/question_2.png");
    success &= preload("question_3", tilePath + "blocks/question_3.png");
    success &= preload("question_4", tilePath + "blocks/question_4.png");
    success &= preload("wood", tilePath + "blocks/wood.png");

    // Masks
    success &= preload("black", tilePath + "masks/black.png");
    success &= preload("black_zigzag", tilePath + "masks/black_zigzag.png");

    // Pipes
    success &= preload("pipe_1", tilePath + "pipes/pipe_1.png");
    success &= preload("pipe_2", tilePath + "pipes/pipe_2.png");
    success &= preload("pipe_3", tilePath + "pipes/pipe_3.png");
    success &= preload("pipe_4", tilePath + "pipes/pipe_4.png");

    // Scenery
    success &= preload("bush", tilePath + "scenery/bush.png");
    success &= preload("cloud", tilePath + "scenery/cloud.png");

    // Terrain - Blue
    success &= preload("blue_1", tilePath + "terrain/blue/blue_1.png");
    success &= preload("blue_2", tilePath + "terrain/blue/blue_2.png");
    success &= preload("blue_3", tilePath + "terrain/blue/blue_3.png");
    success &= preload("blue_4", tilePath + "terrain/blue/blue_4.png");
    success &= preload("blue_5", tilePath + "terrain/blue/blue_5.png");
    success &= preload("blue_6", tilePath + "terrain/blue/blue_6.png");
    success &= preload("blue_7", tilePath + "terrain/blue/blue_7.png");
    success &= preload("blue_8", tilePath + "terrain/blue/blue_8.png");
    success &= preload("blue_9", tilePath + "terrain/blue/blue_9.png");

    // Terrain - Grass
    success &= preload("grass_1", tilePath + "terrain/grass/grass_1.png");
    success &= preload("grass_2", tilePath + "terrain/grass/grass_2.png");
    success &= preload("grass_3", tilePath + "terrain/grass/grass_3.png");
    success &= preload("grass_4", tilePath + "terrain/grass/grass_4.png");
    success &= preload("grass_5", tilePath + "terrain/grass/grass_5.png");
    success &= preload("grass_6", tilePath + "terrain/grass/grass_6.png");
    success &= preload("grass_7", tilePath + "terrain/grass/grass_7.png");
    success &= preload("grass_8", tilePath + "terrain/grass/grass_8.png");
    success &= preload("grass_9", tilePath + "terrain/grass/grass_9.png");
    success &= preload("grass_10", tilePath + "terrain/grass/grass_10.png");
    success &= preload("grass_11", tilePath + "terrain/grass/grass_11.png");
    success &= preload("grass_12", tilePath + "terrain/grass/grass_12.png");
    success &= preload("grass_13", tilePath + "terrain/grass/grass_13.png");
    success &= preload("grass_14", tilePath + "terrain/grass/grass_14.png");
    success &= preload("grass_15", tilePath + "terrain/grass/grass_15.png");
    success &= preload("grass_16", tilePath + "terrain/grass/grass_16.png");
    success &= preload("grass_17", tilePath + "terrain/grass/grass_17.png");
    success &= preload("grass_18", tilePath + "terrain/grass/grass_18.png");

    // Terrain - Orange
    success &= preload("orange_1", tilePath + "terrain/orange/orange_1.png");
    success &= preload("orange_2", tilePath + "terrain/orange/orange_2.png");
    success &= preload("orange_3", tilePath + "terrain/orange/orange_3.png");
    success &= preload("orange_4", tilePath + "terrain/orange/orange_4.png");
    success &= preload("orange_5", tilePath + "terrain/orange/orange_5.png");
    success &= preload("orange_6", tilePath + "terrain/orange/orange_6.png");
    success &= preload("orange_7", tilePath + "terrain/orange/orange_7.png");
    success &= preload("orange_8", tilePath + "terrain/orange/orange_8.png");
    success &= preload("orange_9", tilePath + "terrain/orange/orange_9.png");

    // Terrain - Wood
    success &= preload("wood_0", tilePath + "terrain/wood/wood_0.png");
    success &= preload("wood_1", tilePath + "terrain/wood/wood_1.png");
    success &= preload("wood_2", tilePath + "terrain/wood/wood_2.png");
    success &= preload("wood_3", tilePath + "terrain/wood/wood_3.png");
    success &= preload("wood_4", tilePath + "terrain/wood/wood_4.png");
    success &= preload("wood_5", tilePath + "terrain/wood/wood_5.png");
    return success;
}

bool TextureManager::uiTextureLoad() {
    const std::string uiPath = "assets/ui/";
    bool success = true;
    // HUD
    success &= preload("ui.hud.life", uiPath + "hud/life.png");
    success &= preload("ui.hud.score_100", uiPath + "hud/score_100.png");

    // End box
    success &= preload("ui.end_box.default", uiPath + "end_box/default.png");
    success &= preload("ui.end_box.flower", uiPath + "end_box/flower.png");
    success &= preload("ui.end_box.mushroom", uiPath + "end_box/mushroom.png");
    success &= preload("ui.end_box.star", uiPath + "end_box/star.png");
    return success;
}

SDL_Texture* TextureManager::getTexture(const std::string& id) const {
    auto it = textureMap.find(id);
    if (it != textureMap.end()) {
        return it->second;
    }
    std::cerr << "[TextureManager] Key '" << id << "' not found.\n";
    return nullptr;
}

void TextureManager::clean() {
    for (auto& pair : textureMap) {
        if (pair.second != nullptr) {
            SDL_DestroyTexture(pair.second);
        }
    }
    textureMap.clear();
}

bool TextureManager::gameTextureLoad() {
    bool success = true;
    success &= backgroundTextureLoad();
    success &= playerTextureLoad();
    success &= effectTextureLoad();
    success &= enemyTextureLoad();
    success &= itemTextureLoad();
    success &= tileTextureLoad();
    success &= uiTextureLoad();
    return success;
}

TextureManager::~TextureManager() {
    clean();
}

