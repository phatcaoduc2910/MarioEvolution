#include "model/Brick.h"
#include "model/Item.h"
#include "model/Player.h"
#include <algorithm>

// Tạo brick kích thước 32 x 32 với trạng thái breakable
Brick::Brick(double x, double y, bool breakable)
    : StaticObject(x, y, 32, 32),
      breakable(breakable),
      opened(false){}

/*
    Xử lý khi Player đập Brick từ bên dưới
    Brick thường chỉ bị phá khi Player không trong trạng thái Small hoặc Dead
*/ 
void Brick::hitBy(Player& player){
    if(opened){
        return; // Nếu đã mở hoặc phá rồi thì không xử lý lại nữa
    }

    // TH gạch thường: Phá được khi player không Small hoặc Dead (Big hoặc Fire)
    if(breakable){
        if(player.getState() != PlayerState::Small &&
           player.getState() != PlayerState::Dead){
                opened = true;  // Đánh dấu đã phá
                solid = false;  // Không chặn Player nữa
           }
           return;
    }

    // SpecialBrick được CollisionSystem nhận diện và gọi releaseItem() trực tiếp
}

// Trả về true nếu Brick phá được, ngược lại là false
bool Brick::canBeBroken() const{
    return breakable;
}

// Trả về true nếu Brick được mở hoặc phá, ngược lại là false 
bool Brick::isOpened() const{
    return opened;
}

// Khởi tạo một Brick có thể phá
StandardBrick::StandardBrick(double x, double y)
    :Brick(x, y, true){}

// Nếu chưa phá -> Đánh dấu đã phá, tắt collision
void StandardBrick::breakBrick(){
    if(!opened){
        opened = true;
        solid = false;
    }
}

// Khởi tạo Brick đặc biệt chứa một loại Item
SpecialBrick::SpecialBrick(double x, double y, ItemType content)
    :Brick(x, y, false),
    content(content){}

std::unique_ptr<Item> SpecialBrick::releaseItem(){
    if(opened){
        return nullptr; // Nếu mở rồi thì không tạo Item nữa
    }

    std::unique_ptr<Item> item;
    switch(content){
        case ItemType::Coin:
            item = std::make_unique<Coin>(x + (width - 16) / 2.0, y - 16, 1);
            break;
        case ItemType::Mushroom:
            item = std::make_unique<Mushroom>(x, y - 32);
            break;
        case ItemType::FireFlower:
            item = std::make_unique<FireFlower>(x, y - 32);
            break;
    }

    open();
    return item;
}

// Đánh dấu Special Brick đã được mở
void SpecialBrick::open(){
    opened = true;
}

// Khởi tạo coin brick với số coin không âm
CoinBrick::CoinBrick(double x, double y, int coinAmount)
    :SpecialBrick(x, y, ItemType::Coin),
     coinAmount(std::max(0, coinAmount)){}

    
// Nhả ra một coin và giảm số coin còn lại
std::unique_ptr<Item> CoinBrick::releaseItem(){
    if(coinAmount <= 0){
        open();
        return nullptr;
    }

    auto coin = std::make_unique<Coin>(x + (width - 16) / 2.0, y - 16, 1);
    --coinAmount;
    if(coinAmount == 0){
        open();
    }
    return coin;
}

// Khởi tạo brick chứa mushroom
MushroomBrick::MushroomBrick(double x, double y)
    : SpecialBrick(x, y, ItemType::Mushroom){}

// Giải phóng Mushroom ở lần mở đầu tiên
std::unique_ptr<Item> MushroomBrick::releaseItem(){
    if(opened){
        return nullptr;
    }

    auto mushroom = std::make_unique<Mushroom>(x, y - 32);  // Tạo mushroom
    open();                                                 // Mở brick
    return mushroom;                                        // Chuyển item cho CollisionSystem
}

// Khởi tạo brick chứa fire flower
FlowerBrick::FlowerBrick(double x, double y)
    : SpecialBrick(x, y, ItemType::FireFlower){}

// Giải phóng fire flower ở lần mở đầu tiên
std::unique_ptr<Item> FlowerBrick::releaseItem(){
    if(opened){
        return nullptr;
    }

    auto flower = std::make_unique<FireFlower>(x, y - 32);
    open();
    return flower;
}