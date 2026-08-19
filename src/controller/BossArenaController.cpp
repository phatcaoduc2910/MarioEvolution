#include "controller/BossArenaController.h"

#include "model/Boss.h"
#include "model/Enemy.h"
#include "model/World.h"

#include <algorithm>
#include <vector>

namespace {
constexpr double kPiranhaIntervalSeconds = 4.50;
constexpr double kKoopaDropIntervalSeconds = 5.00;
// Cho người chơi vài giây đọc arena trước đợt hazard đầu tiên của Phase 3.
constexpr double kFirstWaveDelaySeconds = 2.00;
// Boss chỉ được đi lại ở nửa arena phía nó: Mario luôn còn chỗ né và lấy đà
// đá shell thay vì bị dồn vào góc.
constexpr double kBossRegionStartRatio = 0.45;
}

BossArenaController::BossArenaController(unsigned seed)
    : fightStarted(false),
      fightFinished(false),
      hazardsCleared(false),
      phase(BossPhase::Phase1),
      piranhaTimerSeconds(kFirstWaveDelaySeconds),
      koopaTimerSeconds(kFirstWaveDelaySeconds),
      piranhaIntervalSeconds(kPiranhaIntervalSeconds),
      koopaDropIntervalSeconds(kKoopaDropIntervalSeconds),
      randomEngine(seed) {}

void BossArenaController::reset() {
    fightStarted = false;
    fightFinished = false;
    hazardsCleared = false;
    phase = BossPhase::Phase1;
    piranhaTimerSeconds = kFirstWaveDelaySeconds;
    koopaTimerSeconds = kFirstWaveDelaySeconds;
}

bool BossArenaController::isFightStarted() const {
    return fightStarted;
}

bool BossArenaController::isFightFinished() const {
    return fightFinished;
}

bool BossArenaController::isPhase3ScheduleActive() const {
    return fightStarted && !fightFinished && phase == BossPhase::Phase3;
}

BossPhase BossArenaController::getPhase() const {
    return phase;
}

void BossArenaController::setPiranhaIntervalSeconds(double seconds) {
    piranhaIntervalSeconds = std::max(0.5, seconds);
}

void BossArenaController::setKoopaDropIntervalSeconds(double seconds) {
    koopaDropIntervalSeconds = std::max(0.5, seconds);
}

void BossArenaController::update(World& world, double dtSeconds) {
    GorillaBoss* boss = world.getBoss();
    if (boss == nullptr) {
        // Map thường không có boss: arena controller đứng yên hoàn toàn.
        return;
    }

    if (!fightStarted) {
        // Fight chỉ start một lần cho mỗi lần nạp arena.
        fightStarted = true;
        const double arenaMinX = world.getArenaMinX();
        const double arenaMaxX = world.getArenaMaxX();
        boss->setMovementRegion(
            arenaMinX + (arenaMaxX - arenaMinX) * kBossRegionStartRatio,
            arenaMaxX);
    }
    phase = boss->getPhase();

    if (boss->isDeathStarted()) {
        if (!hazardsCleared) {
            world.clearHazards();
            hazardsCleared = true;
        }

        if (boss->isDeathFinished() && !fightFinished) {
            fightFinished = true;
            world.markLevelComplete();
        }
        return;
    }

    if (phase == BossPhase::Phase3) {
        updatePhase3Hazards(world, dtSeconds);
    }
}

// Phase 1/2 không có hazard phụ; Phase 3 mới bật Piranha và Koopa rơi.
void BossArenaController::updatePhase3Hazards(World& world, double dtSeconds) {
    piranhaTimerSeconds -= dtSeconds;
    if (piranhaTimerSeconds <= 0.0) {
        piranhaTimerSeconds = piranhaIntervalSeconds;
        spawnPiranhaWave(world);
    }

    koopaTimerSeconds -= dtSeconds;
    if (koopaTimerSeconds <= 0.0) {
        koopaTimerSeconds = koopaDropIntervalSeconds;
        spawnKoopaWave(world);
    }
}

int BossArenaController::rollWaveSize() {
    std::uniform_int_distribution<int> waveSize(1, 2);
    return waveSize(randomEngine);
}

void BossArenaController::spawnPiranhaWave(World& world) {
    const std::size_t pointCount = world.getPiranhaSpawnPoints().size();
    if (pointCount == 0) {
        return;
    }

    const int budget =
        kMaxActivePiranhas - world.countActivePiranhas();
    if (budget <= 0) {
        return;
    }

    // Chọn ngẫu nhiên 1-2 điểm khác nhau trong các điểm map đã khai báo.
    std::vector<std::size_t> candidates(pointCount);
    for (std::size_t index = 0; index < pointCount; ++index) {
        candidates[index] = index;
    }
    std::shuffle(candidates.begin(), candidates.end(), randomEngine);

    const int waveSize = std::min({rollWaveSize(), budget,
                                   static_cast<int>(pointCount)});
    for (int spawned = 0; spawned < waveSize; ++spawned) {
        world.activatePiranhaAt(candidates[static_cast<std::size_t>(spawned)]);
    }
}

void BossArenaController::spawnKoopaWave(World& world) {
    const int budget = kMaxActiveKoopas - world.countActiveKoopas();
    if (budget <= 0) {
        return;
    }

    const double minX = world.getArenaMinX();
    const double maxX = world.getArenaMaxX() - Koopa::kWalkWidth;
    if (maxX <= minX) {
        return;
    }

    const int waveSize = std::min(rollWaveSize(), budget);
    std::uniform_real_distribution<double> dropX(minX, maxX);
    for (int spawned = 0; spawned < waveSize; ++spawned) {
        world.spawnFallingKoopa(dropX(randomEngine));
    }
}
