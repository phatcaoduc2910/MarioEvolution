#pragma once

#include "core/Types.h"

#include <cstddef>
#include <random>

class World;

// Điều phối boss arena: bật hazard theo BossPhase, dọn sạch khi boss chết và
// chốt level complete. Controller không tính damage boss và không chứa
// behavior của Koopa/Piranha.
class BossArenaController {
public:
    static constexpr int kMaxActivePiranhas = 2;
    static constexpr int kMaxActiveKoopas = 4;

    explicit BossArenaController(unsigned seed = 20260819U);

    void reset();
    void update(World& world, double dtSeconds);

    bool isFightStarted() const;
    bool isFightFinished() const;
    bool isPhase3ScheduleActive() const;
    BossPhase getPhase() const;

    // Tuning gom một chỗ để chỉnh độ khó (C9).
    void setPiranhaIntervalSeconds(double seconds);
    void setKoopaDropIntervalSeconds(double seconds);

private:
    void updatePhase3Hazards(World& world, double dtSeconds);
    void spawnPiranhaWave(World& world);
    void spawnKoopaWave(World& world);
    int rollWaveSize();

    bool fightStarted;
    bool fightFinished;
    bool hazardsCleared;
    BossPhase phase;
    double piranhaTimerSeconds;
    double koopaTimerSeconds;
    double piranhaIntervalSeconds;
    double koopaDropIntervalSeconds;
    std::mt19937 randomEngine;
};
