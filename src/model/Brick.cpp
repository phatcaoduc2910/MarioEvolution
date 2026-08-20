#include "model/Brick.h"
#include "model/Item.h"
#include "model/Player.h"
#include <algorithm>
#include <cmath>

namespace {
constexpr double kBumpDurationSeconds = 0.16;
constexpr double kBumpPeakSeconds = kBumpDurationSeconds / 2.0;
constexpr double kBumpHeightPixels = 8.0;
}

// Tạo brick kích thước 32 x 32 với trạng thái breakable
Brick::Brick(double x, double y, bool breakable)
    : StaticObject(x, y, 32, 32),
      breakable(breakable),
      state(State::Active),
      bumpElapsedSeconds(0.0),
      renderOffsetY(0),
      bumping(false) {}

/*
    Xử lý khi Player đập Brick từ bên dưới
    Brick thường chỉ bị phá khi Player không trong trạng thái Small hoặc Dead
*/ 
void Brick::hitBy(Player& player){
    if (!isActive() || !player.isAlive()) {
        return; // Nếu đã mở hoặc phá rồi thì không xử lý lại nữa
    }

    startBump();

    // TH gạch thường: Phá được khi player không Small hoặc Dead (Big hoặc Fire)
    if (breakable && player.getState() != PlayerState::Small) {
        markBroken();
    }
}

// Cập nhật hiệu ứng gạch nảy sau khi bị đập.
void Brick::update(double dtSeconds) {
    if (!bumping || dtSeconds <= 0.0) {
        return;
    }

    bumpElapsedSeconds = std::min(
        kBumpDurationSeconds, bumpElapsedSeconds + dtSeconds);
    const double height = bumpElapsedSeconds <= kBumpPeakSeconds
                              ? bumpElapsedSeconds / kBumpPeakSeconds
                              : (kBumpDurationSeconds - bumpElapsedSeconds) /
                                    kBumpPeakSeconds;
    renderOffsetY = -static_cast<int>(
        std::lround(kBumpHeightPixels * std::max(0.0, height)));

    if (bumpElapsedSeconds >= kBumpDurationSeconds) {
        bumping = false;
        renderOffsetY = 0;
    }
}

// Trả về true nếu Brick phá được, ngược lại là false
bool Brick::canBeBroken() const{
    return breakable;
}

// Trả về true nếu Brick được mở hoặc phá, ngược lại là false 
bool Brick::isOpened() const{
    return state != State::Active;
}

int Brick::getRenderOffsetY() const {
    return renderOffsetY;
}

bool Brick::isActive() const {
    return state == State::Active;
}

void Brick::markUsed() {
    state = State::Used;
}

void Brick::markBroken() {
    state = State::Broken;
    solid = false;
}

// Bắt đầu hiệu ứng gạch nảy.
void Brick::startBump() {
    if (!bumping) {
        bumping = true;
        bumpElapsedSeconds = 0.0;
    }
}

// Khởi tạo một Brick có thể phá
StandardBrick::StandardBrick(double x, double y)
    : Brick(x, y, true) {}

// Khởi tạo Brick đặc biệt chứa Item
SpecialBrick::SpecialBrick(double x, double y)
    : Brick(x, y, false) {}

// Khởi tạo coin brick với số coin không âm
CoinBrick::CoinBrick(double x, double y, int coinAmount)
    : SpecialBrick(x, y),
      coinAmount(std::max(0, coinAmount)) {}

    
// Nhả ra một coin và giảm số coin còn lại
std::unique_ptr<Item> CoinBrick::releaseItem(){
    if(coinAmount <= 0){
        markUsed();
        return nullptr;
    }

    auto coin = std::make_unique<Coin>(
        x + (width - 16) / 2.0, y - 16, Coin::kScoreValue);
    --coinAmount;
    if(coinAmount == 0){
        markUsed();
    }
    return coin;
}

// Khởi tạo brick chứa mushroom
MushroomBrick::MushroomBrick(double x, double y)
    : SpecialBrick(x, y) {}

// Giải phóng Mushroom ở lần mở đầu tiên
std::unique_ptr<Item> MushroomBrick::releaseItem(){
    if (!isActive()) {
        return nullptr;
    }

    auto mushroom = std::make_unique<Mushroom>(x, y - 32);  // Tạo mushroom
    markUsed();                                             // Mở brick
    return mushroom;                                        // Chuyển item cho CollisionSystem
}

// Khởi tạo brick chứa fire flower
FlowerBrick::FlowerBrick(double x, double y)
    : SpecialBrick(x, y) {}

// Giải phóng fire flower ở lần mở đầu tiên
std::unique_ptr<Item> FlowerBrick::releaseItem(){
    if (!isActive()) {
        return nullptr;
    }

    auto flower = std::make_unique<FireFlower>(x, y - 32);
    markUsed();
    return flower;
}
