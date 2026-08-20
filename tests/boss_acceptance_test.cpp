// Acceptance test cho boss battle: bot chỉ dùng input hợp lệ của người chơi
// (đi trái/phải, nhảy) để chạy trọn vòng phản đòn ném rùa -> đạp -> đá mai.
// Không cheat, không gọi thẳng onShellHit, không dịch chuyển Mario.
#include "controller/BossArenaController.h"
#include "controller/CollisionSystem.h"
#include "model/Boss.h"
#include "model/Enemy.h"
#include "model/LevelData.h"
#include "model/Player.h"
#include "model/World.h"
#include "service/LevelCodec.h"

#include <cassert>
#include <cmath>
#include <cstdlib>
#include <iostream>

namespace {
constexpr double kStepSeconds = 0.011;
constexpr int kTileSize = 32;
constexpr double kMaxFightSeconds = 240.0;
// Khoảng cách an toàn khi không có việc gì làm với boss.
constexpr double kSafeDistance = 220.0;
constexpr double kStompJumpPixels = 70.0;
constexpr double kKickStandOffset = 30.0;

double centerX(const GameObject& object) {
    return object.getX() + object.getWidth() / 2.0;
}

Koopa* findKoopa(World& world, bool wantShell) {
    Koopa* best = nullptr;
    for (const auto& actor : world.getActors()) {
        auto* koopa = dynamic_cast<Koopa*>(actor.get());
        if (koopa == nullptr || !koopa->isAlive() || koopa->isSlidingShell()) {
            continue;
        }
        if (koopa->isShell() != wantShell || !koopa->isOnGround()) {
            continue;
        }
        if (best == nullptr || koopa->getY() > best->getY()) {
            best = koopa;
        }
    }
    return best;
}

// Ưu tiên cao nhất: tránh Piranha đang nhô lên và cú Charge của boss.
int avoidDirection(World& world, const Player& player, const GorillaBoss& boss) {
    const double playerX = centerX(player);

    for (const auto& actor : world.getActors()) {
        const auto* plant = dynamic_cast<const PiranhaPlant*>(actor.get());
        if (plant == nullptr || !plant->isAlive() || plant->getHeight() <= 0) {
            continue;
        }
        const double distance = playerX - centerX(*plant);
        if (std::abs(distance) < 70.0) {
            return distance > 0.0 ? 1 : -1;
        }
    }

    if (boss.getAction() == BossAction::Charge && boss.isAttackActive()) {
        const double distance = playerX - centerX(boss);
        if (std::abs(distance) < 200.0) {
            return distance > 0.0 ? 1 : -1;
        }
    }

    return 0;
}

void driveBot(World& world, const GorillaBoss& boss) {
    Player& player = world.getPlayer();
    if (!player.isAlive()) {
        player.setMoveDirection(0);
        return;
    }

    const double playerX = centerX(player);
    const double bossX = centerX(boss);

    if (const int escape = avoidDirection(world, player, boss); escape != 0) {
        player.setMoveDirection(escape);
        return;
    }

    if (Koopa* shell = findKoopa(world, true)) {
        // Đứng ở phía đối diện boss rồi đi vào mai để đá nó về phía boss.
        const bool kickRight = shell->getX() < bossX;
        const double standX = kickRight
                                  ? shell->getX() - kKickStandOffset
                                  : shell->getX() + shell->getWidth() +
                                        kKickStandOffset;
        const double delta = standX - playerX;
        if (std::abs(delta) > 6.0) {
            player.setMoveDirection(delta > 0.0 ? 1 : -1);
        } else {
            player.setMoveDirection(kickRight ? 1 : -1);
        }
        return;
    }

    if (Koopa* walker = findKoopa(world, false)) {
        const double delta = centerX(*walker) - playerX;
        const double gap = std::abs(delta);
        const int toward = delta > 0.0 ? 1 : -1;

        // Bám theo rùa và nhảy sớm để rơi trúng đầu thay vì va ngang.
        player.setMoveDirection(toward);
        if (player.isOnGround() && gap < kStompJumpPixels) {
            player.jump();
        }
        return;
    }

    const double distanceToBoss = playerX - bossX;
    if (std::abs(distanceToBoss) < kSafeDistance) {
        player.setMoveDirection(distanceToBoss > 0.0 ? 1 : -1);
        return;
    }
    player.setMoveDirection(0);
}
}

int main() {
    const LevelData level =
        LevelCodec::load("assets/maps/boss_arena.map", kTileSize);
    World world;
    world.loadLevel(level);
    CollisionSystem collisions;
    BossArenaController arena;

    GorillaBoss* boss = world.getBoss();
    if (boss == nullptr) {
        std::cerr << "boss_arena.map khong co boss\n";
        return 1;
    }

    int lastHp = boss->getCurrentHp();
    int lastLives = world.getLives();
    double elapsed = 0.0;
    double phase1Seconds = -1.0;
    while (elapsed < kMaxFightSeconds && !world.isLevelComplete() &&
           !world.isGameOver()) {
        driveBot(world, *boss);
        world.update(kStepSeconds, world.getPlayer().getX());
        collisions.update(world, kStepSeconds);
        arena.update(world, kStepSeconds);
        elapsed += kStepSeconds;

        if (world.getLives() != lastLives) {
            lastLives = world.getLives();
            const Player& player = world.getPlayer();
            std::cerr << "  mat mang t=" << elapsed << " x=" << player.getX()
                      << " y=" << player.getY() << " bossAction="
                      << static_cast<int>(boss->getAction()) << " bossX="
                      << boss->getX() << '\n';
            for (const auto& actor : world.getActors()) {
                if (std::abs(centerX(*actor) - centerX(player)) > 90.0) {
                    continue;
                }
                const auto* plant =
                    dynamic_cast<const PiranhaPlant*>(actor.get());
                const auto* koopa = dynamic_cast<const Koopa*>(actor.get());
                std::cerr << "    gan: "
                          << (plant != nullptr ? "piranha"
                              : koopa != nullptr ? "koopa"
                                                 : "khac")
                          << " x=" << actor->getX() << " y=" << actor->getY()
                          << " h=" << actor->getHeight() << '\n';
            }
        }

        if (boss->getCurrentHp() != lastHp) {
            lastHp = boss->getCurrentHp();
            std::cerr << "t=" << elapsed << "s boss HP=" << lastHp
                      << " lives=" << world.getLives() << '\n';
            if (lastHp == 3 && phase1Seconds < 0.0) {
                phase1Seconds = elapsed;
            }
        }
    }

    std::cerr << "ket thuc t=" << elapsed << "s HP=" << boss->getCurrentHp()
              << " lives=" << world.getLives()
              << " levelComplete=" << world.isLevelComplete() << '\n';

    assert(phase1Seconds > 0.0 && "Phase 1 phai ha duoc 3 HP bang phan don");
    assert(boss->getCurrentHp() == 0 && "Boss phai chet bang shell counter");
    assert(boss->isDeathFinished());
    assert(world.isLevelComplete());
    assert(!world.isGameOver());
    assert(world.countActivePiranhas() == 0);
    assert(world.countActiveKoopas() == 0);

    std::cerr << "Boss battle acceptance passed\n";
    return 0;
}
