#pragma once

#include "Enemy.h"

#include <cstdint>
#include <random>
#include <unordered_set>

class Koopa;

// Boss gorilla: chỉ ShellSliding hợp lệ mới trừ máu, mỗi shell tối đa một lần.
// Lớp này giữ HP/phase/action; nó không tự tạo Koopa, không load texture và
// không quyết định lịch hazard của arena.
class GorillaBoss : public Enemy {
public:
    static constexpr int kMaxHp = 6;
    static constexpr int kBossWidth = 84;
    static constexpr int kBossHeight = 88;
    static constexpr int kMaxConsecutiveDodges = 2;

    GorillaBoss(double x, double y, unsigned seed = 20260819U);

    int getCurrentHp() const;
    int getMaxHp() const;
    BossPhase getPhase() const;
    BossAction getAction() const;
    BossAttackStage getAttackStage() const;
    // Charge/GroundSlam chỉ nguy hiểm ở stage Active, Prepare là telegraph.
    bool isAttackActive() const;
    bool isDeathStarted() const;
    bool isDeathFinished() const;
    int getConsecutiveDodges() const;

    // Điểm tích hợp duy nhất cho collision layer (A1).
    BossHitResult onShellHit(const Koopa& shell);

    // Boss chỉ phát yêu cầu ném; World/B tạo Koopa thật (A7).
    bool consumeThrowRequest();
    Direction getThrowDirection() const;
    // Cue cho hiệu ứng: World phát VisualEvent, boss không tự spawn effect.
    bool consumeSlamImpactCue();
    bool consumeDodgeCue();
    bool consumeDeathCue();

    // Arena (C) quyết định vùng di chuyển và vị trí Mario cần bám.
    void setMovementRegion(double minX, double maxX);
    void trackPlayerCenter(double playerCenterX);
    void setDodgeChance(double chance);
    double getDodgeChance() const;

    void update(double dtSeconds) override;
    void patrol() override;
    void die() override;
    void onPlayerContact(Player& player) override;
    void onStomped(Player& player) override;
    bool isStompable() const override;
    bool shouldTurnAtEdge() const override;
    bool takesFireballDamage() const override;
    bool isRemovable() const override;

private:
    void startAction(BossAction nextAction, BossAttackStage nextStage,
                     double durationSeconds);
    void advanceAction();
    void beginNextDecision();
    void applyShellDamage();
    void startDeath();
    void refreshPhase();
    bool canDodge() const;
    bool rollDodge();
    void faceTowardsPlayer();
    void clampToRegion();

    int currentHp;
    BossPhase phase;
    BossPhase announcedPhase;
    BossAction action;
    BossAttackStage stage;
    double actionElapsedSeconds;
    double actionDurationSeconds;
    double throwCooldownSeconds;
    double meleeCooldownSeconds;
    double hurtWindowSeconds;
    double dodgeCooldownSeconds;
    double deathElapsedSeconds;
    double dodgeChance;
    double playerCenterX;
    double movementMinX;
    double movementMaxX;
    int consecutiveDodges;
    bool pendingThrow;
    bool pendingSlamImpact;
    bool pendingDodgeCue;
    bool pendingDeathCue;
    bool deathStarted;
    bool deathFinished;
    bool nextMeleeIsCharge;
    std::unordered_set<std::uint32_t> decidedShellIds;
    std::mt19937 randomEngine;
};
