#include "controller/BossArenaController.h"
#include "controller/CollisionSystem.h"
#include "model/Boss.h"
#include "model/Enemy.h"
#include "model/Fireball.h"
#include "model/LevelData.h"
#include "model/World.h"
#include "service/LevelCodec.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <memory>

namespace {
constexpr double kStepSeconds = 0.011;
constexpr int kTileSize = 32;
constexpr int kArenaWidth = 25;
constexpr int kArenaHeight = 19;
constexpr int kFloorRow = 17;
constexpr int kGroundRow = kFloorRow - 1;

// Arena tối thiểu: hai hàng gạch làm sàn, marker spawn cho Mario và boss.
LevelData makeArenaLevel() {
    LevelData level(kArenaWidth, kArenaHeight, kTileSize);
    for (int column = 0; column < kArenaWidth; ++column) {
        for (int row = kFloorRow; row < kArenaHeight; ++row) {
            level.setTile(column, row, kStandardBrickTileId);
        }
    }
    for (int row = 0; row < kFloorRow; ++row) {
        level.setTile(0, row, kStandardBrickTileId);
        level.setTile(kArenaWidth - 1, row, kStandardBrickTileId);
    }
    level.setTile(3, kGroundRow, kPlayerSpawnTileId);
    level.setTile(19, kGroundRow, kBossSpawnTileId);
    for (const int column : {5, 9, 13, 17}) {
        level.setTile(column, kGroundRow, kPiranhaSpawnPointTileId);
    }
    return level;
}

void step(World& world, CollisionSystem& collisions, double seconds,
          BossArenaController* arena = nullptr) {
    const int steps = static_cast<int>(seconds / kStepSeconds);
    for (int index = 0; index < steps; ++index) {
        world.update(kStepSeconds, world.getPlayer().getX());
        collisions.update(world, kStepSeconds);
        if (arena != nullptr) {
            arena->update(world, kStepSeconds);
        }
    }
}

// Đánh boss bằng shell trực tiếp qua điểm tích hợp của A; mỗi lần một shell mới.
void hitBossWithFreshShell(GorillaBoss& boss, BossHitResult expected) {
    Koopa shell(0.0, 0.0, KoopaColor::Green);
    assert(boss.onShellHit(shell) == expected);
}

void damageBossTo(GorillaBoss& boss, int targetHp) {
    while (boss.getCurrentHp() > targetHp) {
        hitBossWithFreshShell(boss, BossHitResult::Damaged);
        // Cửa sổ hurt phải hết trước khi shell kế tiếp được tính.
        boss.update(0.6);
    }
}

void testShellHitRemovesExactlyOneHp() {
    GorillaBoss boss(600.0, 456.0);
    assert(boss.getCurrentHp() == GorillaBoss::kMaxHp);
    assert(boss.getPhase() == BossPhase::Phase1);

    hitBossWithFreshShell(boss, BossHitResult::Damaged);
    assert(boss.getCurrentHp() == 5);
    assert(boss.getAction() == BossAction::Hurt);
}

void testOneShellCannotMultiHit() {
    GorillaBoss boss(600.0, 456.0);
    Koopa shell(0.0, 0.0, KoopaColor::Green);

    assert(boss.onShellHit(shell) == BossHitResult::Damaged);
    assert(boss.onShellHit(shell) == BossHitResult::Ignored);
    boss.update(1.0);
    // Kể cả khi cửa sổ hurt đã hết, shell cũ vẫn không được tính lần hai.
    assert(boss.onShellHit(shell) == BossHitResult::Ignored);
    assert(boss.getCurrentHp() == 5);
}

void testHurtWindowBlocksSecondShellInSameBeat() {
    GorillaBoss boss(600.0, 456.0);

    hitBossWithFreshShell(boss, BossHitResult::Damaged);
    hitBossWithFreshShell(boss, BossHitResult::Ignored);
    assert(boss.getCurrentHp() == 5);
}

void testPhaseThresholdsTriggerOnce() {
    GorillaBoss boss(600.0, 456.0);

    damageBossTo(boss, 4);
    assert(boss.getPhase() == BossPhase::Phase1);

    hitBossWithFreshShell(boss, BossHitResult::Damaged);
    assert(boss.getCurrentHp() == 3);
    assert(boss.getPhase() == BossPhase::Phase2);
    // Cue Enraged chỉ phát tại ngưỡng, hit tiếp theo quay về Hurt.
    assert(boss.getAction() == BossAction::Enraged);
    boss.update(0.6);

    hitBossWithFreshShell(boss, BossHitResult::Damaged);
    assert(boss.getCurrentHp() == 2);
    assert(boss.getPhase() == BossPhase::Phase2);
    assert(boss.getAction() == BossAction::Hurt);
    boss.update(0.6);

    hitBossWithFreshShell(boss, BossHitResult::Damaged);
    assert(boss.getCurrentHp() == 1);
    assert(boss.getPhase() == BossPhase::Phase3);
    assert(boss.getAction() == BossAction::Enraged);
}

void testPhase1NeverDodges() {
    GorillaBoss boss(600.0, 456.0);
    boss.setDodgeChance(1.0);

    hitBossWithFreshShell(boss, BossHitResult::Damaged);
    assert(boss.getCurrentHp() == 5);
    assert(boss.getConsecutiveDodges() == 0);
}

void testDodgeDecidedOncePerShell() {
    GorillaBoss boss(600.0, 456.0);
    boss.setDodgeChance(1.0);
    damageBossTo(boss, 1);
    assert(boss.getPhase() == BossPhase::Phase3);

    Koopa shell(0.0, 0.0, KoopaColor::Green);
    assert(boss.onShellHit(shell) == BossHitResult::Dodged);
    // Shell đã bị né thì không được random lại ở các frame sau.
    for (int frame = 0; frame < 20; ++frame) {
        boss.update(kStepSeconds);
        assert(boss.onShellHit(shell) == BossHitResult::Ignored);
    }
    assert(boss.getCurrentHp() == 1);
    assert(boss.getAction() == BossAction::Dodge);
}

void testDodgeStopsAfterTwoInARow() {
    GorillaBoss boss(600.0, 456.0);
    boss.setDodgeChance(1.0);
    damageBossTo(boss, 1);

    hitBossWithFreshShell(boss, BossHitResult::Dodged);
    boss.update(2.0);
    hitBossWithFreshShell(boss, BossHitResult::Dodged);
    boss.update(2.0);
    assert(boss.getConsecutiveDodges() == GorillaBoss::kMaxConsecutiveDodges);

    // Shell thứ ba bắt buộc phải có cửa trúng, nếu không fight bị RNG khoá.
    hitBossWithFreshShell(boss, BossHitResult::Damaged);
    assert(boss.getCurrentHp() == 0);
    assert(boss.getConsecutiveDodges() == 0);
}

void testDeathLifecycleStopsAttacks() {
    GorillaBoss boss(600.0, 456.0);
    damageBossTo(boss, 0);

    assert(boss.getPhase() == BossPhase::Dead);
    assert(boss.isDeathStarted());
    assert(boss.getAction() == BossAction::Death);
    assert(!boss.isDeathFinished());

    for (int frame = 0; frame < 60; ++frame) {
        boss.update(kStepSeconds);
        assert(!boss.consumeThrowRequest());
        assert(boss.getAction() == BossAction::Death);
    }

    boss.update(1.5);
    assert(boss.isDeathFinished());
    // Xác boss ở lại arena để C chạy xong death sequence.
    assert(!boss.isRemovable());
    hitBossWithFreshShell(boss, BossHitResult::Ignored);
}

void testBossThrowsKoopaThatLandsAndWalks() {
    World world;
    LevelData level = makeArenaLevel();
    world.loadLevel(level);
    CollisionSystem collisions;

    GorillaBoss* boss = world.getBoss();
    assert(boss != nullptr);
    const std::size_t actorsBefore = world.getActors().size();

    // Phase 1 ném theo timer ~3.4 s, chạy đủ dài để có một cú ném.
    step(world, collisions, 5.0);
    assert(world.getActors().size() > actorsBefore);

    const Koopa* thrown = nullptr;
    for (const auto& actor : world.getActors()) {
        const auto* koopa = dynamic_cast<const Koopa*>(actor.get());
        if (koopa != nullptr) {
            thrown = koopa;
            break;
        }
    }
    assert(thrown != nullptr);

    step(world, collisions, 2.0);
    assert(thrown->isOnGround());
    assert(!thrown->isAirborne());
    // Chạm đất xong Koopa quay lại behavior thường để Mario đạp thành mai.
    assert(thrown->getVelocityX() != 0.0);
    assert(thrown->isAlive());
}

void testFallingKoopaLandsAndBecomesShellAmmo() {
    World world;
    LevelData level = makeArenaLevel();
    world.loadLevel(level);
    CollisionSystem collisions;

    Koopa& falling = world.spawnFallingKoopa(320.0);
    assert(falling.isAirborne());

    step(world, collisions, 3.0);
    assert(falling.isOnGround());
    assert(falling.isAlive());

    falling.hideInShell();
    assert(falling.isShell());
    falling.kick(Direction::Right);
    assert(falling.isSlidingShell());
    assert(falling.isDeadlyToEnemies());
}

void testSlidingShellDamagesBossThroughCollision() {
    World world;
    LevelData level = makeArenaLevel();
    world.loadLevel(level);
    CollisionSystem collisions;

    GorillaBoss* boss = world.getBoss();
    assert(boss != nullptr);

    auto shell = std::make_unique<Koopa>(
        boss->getX() - 120.0,
        static_cast<double>(kFloorRow * kTileSize - Koopa::kWalkHeight),
        KoopaColor::Green);
    shell->hideInShell();
    shell->kick(Direction::Right);
    const std::uint32_t shellId = shell->getId();
    world.addActor(std::move(shell));

    step(world, collisions, 1.0);
    assert(boss->getCurrentHp() == 5);

    // Shell bị tiêu thụ ngay sau cú trúng nên không thể multi-hit.
    for (const auto& actor : world.getActors()) {
        assert(actor->getId() != shellId || !actor->isAlive());
    }
}

void testFireballDoesNotDamageBoss() {
    World world;
    LevelData level = makeArenaLevel();
    world.loadLevel(level);
    CollisionSystem collisions;

    GorillaBoss* boss = world.getBoss();
    assert(boss != nullptr);
    assert(!boss->takesFireballDamage());

    auto fireball = std::make_unique<Fireball>(
        boss->getX() + 10.0, boss->getY() + 20.0, Direction::Right);
    const std::uint32_t fireballId = fireball->getId();
    world.addActor(std::move(fireball));

    step(world, collisions, 0.2);
    assert(boss->getCurrentHp() == GorillaBoss::kMaxHp);
    assert(boss->getPhase() == BossPhase::Phase1);

    // Quả cầu vẫn tắt khi chạm boss, chỉ là không gây damage.
    for (const auto& actor : world.getActors()) {
        assert(actor->getId() != fireballId || !actor->isAlive());
    }
}

void testPiranhaOneShotCycleCleansItself() {
    World world;
    LevelData level = makeArenaLevel();
    world.loadLevel(level);
    CollisionSystem collisions;

    assert(world.getPiranhaSpawnPoints().size() == 4);
    PiranhaPlant* plant = world.activatePiranhaAt(0);
    assert(plant != nullptr);
    assert(plant->isOneShot());
    assert(world.countActivePiranhas() == 1);

    // Hidden 1.8 + Rising 0.45 + Exposed 1.6 + Sinking 0.45 rồi tự chết.
    step(world, collisions, 4.6);
    assert(world.countActivePiranhas() == 0);

    step(world, collisions, 1.0);
    for (const auto& actor : world.getActors()) {
        assert(dynamic_cast<const PiranhaPlant*>(actor.get()) == nullptr);
    }
}

void testArenaMapLoadsBossAndSpawnPoints() {
    const LevelData level =
        LevelCodec::load("assets/maps/boss_arena.map", kTileSize);
    World world;
    world.loadLevel(level);

    assert(world.getBoss() != nullptr);
    assert(world.getPiranhaSpawnPoints().size() == 4);
    assert(world.getArenaMinX() == static_cast<double>(kTileSize));
    assert(world.getArenaMaxX() ==
           static_cast<double>((level.getWidth() - 1) * kTileSize));
}

void testPhase1HasNoArenaHazards() {
    World world;
    LevelData level = makeArenaLevel();
    world.loadLevel(level);
    CollisionSystem collisions;
    BossArenaController arena;

    step(world, collisions, 6.0, &arena);
    assert(arena.isFightStarted());
    assert(!arena.isPhase3ScheduleActive());
    assert(arena.getPhase() == BossPhase::Phase1);
    // Piranha là hazard riêng của Phase 3, Phase 1 tuyệt đối không có.
    assert(world.countActivePiranhas() == 0);
}

void testPhase3EnablesHazardSchedulers() {
    World world;
    LevelData level = makeArenaLevel();
    world.loadLevel(level);
    CollisionSystem collisions;
    BossArenaController arena;

    GorillaBoss* boss = world.getBoss();
    assert(boss != nullptr);
    damageBossTo(*boss, 1);
    assert(boss->getPhase() == BossPhase::Phase3);

    step(world, collisions, 3.0, &arena);
    assert(arena.isPhase3ScheduleActive());
    assert(world.countActivePiranhas() > 0);
    assert(world.countActivePiranhas() <=
           BossArenaController::kMaxActivePiranhas);
    assert(world.countActiveKoopas() > 0);
    assert(world.countActiveKoopas() <=
           BossArenaController::kMaxActiveKoopas + 2);
}

void testBossDeathClearsHazardsAndCompletesLevel() {
    World world;
    LevelData level = makeArenaLevel();
    world.loadLevel(level);
    CollisionSystem collisions;
    BossArenaController arena;

    GorillaBoss* boss = world.getBoss();
    assert(boss != nullptr);
    damageBossTo(*boss, 1);
    step(world, collisions, 3.0, &arena);
    assert(world.countActivePiranhas() > 0);

    hitBossWithFreshShell(*boss, BossHitResult::Damaged);
    assert(boss->isDeathStarted());

    step(world, collisions, 0.5, &arena);
    assert(world.countActivePiranhas() == 0);
    assert(world.countActiveKoopas() == 0);
    assert(!world.isLevelComplete());

    step(world, collisions, 1.5, &arena);
    assert(boss->isDeathFinished());
    assert(arena.isFightFinished());
    assert(world.isLevelComplete());

    // Hết fight thì scheduler không được sinh thêm hazard nào nữa.
    step(world, collisions, 5.0, &arena);
    assert(world.countActivePiranhas() == 0);
    assert(world.countActiveKoopas() == 0);
}

void testRetryResetsArenaState() {
    World world;
    LevelData level = makeArenaLevel();
    world.loadLevel(level);
    CollisionSystem collisions;
    BossArenaController arena;

    GorillaBoss* boss = world.getBoss();
    assert(boss != nullptr);
    damageBossTo(*boss, 0);
    step(world, collisions, 2.0, &arena);
    assert(arena.isFightFinished());

    arena.reset();
    assert(!arena.isFightStarted());
    assert(!arena.isFightFinished());
    assert(arena.getPhase() == BossPhase::Phase1);

    World retried;
    retried.loadLevel(level);
    GorillaBoss* freshBoss = retried.getBoss();
    assert(freshBoss != nullptr);
    assert(freshBoss->getCurrentHp() == GorillaBoss::kMaxHp);
    assert(freshBoss->getPhase() == BossPhase::Phase1);
    assert(!freshBoss->isDeathStarted());
    assert(retried.countActiveKoopas() == 0);
}

void testControllerIgnoresLevelsWithoutBoss() {
    World world;
    LevelData level = makeArenaLevel();
    level.setTile(19, kGroundRow, kEmptyTileId);
    world.loadLevel(level);
    CollisionSystem collisions;
    BossArenaController arena;

    step(world, collisions, 4.0, &arena);
    assert(world.getBoss() == nullptr);
    assert(!arena.isFightStarted());
    assert(world.countActivePiranhas() == 0);
    assert(!world.isLevelComplete());
}
}

int main() {
    testShellHitRemovesExactlyOneHp();
    testOneShellCannotMultiHit();
    testHurtWindowBlocksSecondShellInSameBeat();
    testPhaseThresholdsTriggerOnce();
    testPhase1NeverDodges();
    testDodgeDecidedOncePerShell();
    testDodgeStopsAfterTwoInARow();
    testDeathLifecycleStopsAttacks();
    testBossThrowsKoopaThatLandsAndWalks();
    testFallingKoopaLandsAndBecomesShellAmmo();
    testSlidingShellDamagesBossThroughCollision();
    testFireballDoesNotDamageBoss();
    testPiranhaOneShotCycleCleansItself();
    testArenaMapLoadsBossAndSpawnPoints();
    testPhase1HasNoArenaHazards();
    testPhase3EnablesHazardSchedulers();
    testBossDeathClearsHazardsAndCompletesLevel();
    testRetryResetsArenaState();
    testControllerIgnoresLevelsWithoutBoss();

    std::cout << "Boss battle validation passed\n";
    return 0;
}
