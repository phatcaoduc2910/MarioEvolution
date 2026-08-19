#include "model/Boss.h"

#include "model/Player.h"

#include <algorithm>

namespace {
// Nhịp ra quyết định: Idle chỉ là khoảng chờ ngắn giữa hai hành động.
constexpr double kIdleTickSeconds = 0.20;
constexpr double kWindUpSeconds = 0.50;
constexpr double kThrowSeconds = 0.25;
constexpr double kRecoverSeconds = 0.55;
constexpr double kEnragedSeconds = 0.60;
constexpr double kHurtSeconds = 0.45;
constexpr double kDodgeSeconds = 0.50;
constexpr double kDodgeCooldownSeconds = 1.60;
constexpr double kDeathSeconds = 1.20;

constexpr double kChargePrepareSeconds = 0.45;
constexpr double kChargeActiveSeconds = 0.90;
constexpr double kChargeRecoverSeconds = 0.70;
constexpr double kChargeSpeedPixelsPerSecond = 240.0;

constexpr double kSlamPrepareSeconds = 0.50;
constexpr double kSlamActiveSeconds = 0.40;
constexpr double kSlamRecoverSeconds = 0.80;

// Ném thưa ở Phase 1 để người chơi kịp học cơ chế phản đòn.
constexpr double kThrowIntervalPhase1Seconds = 3.40;
constexpr double kThrowIntervalPhase2Seconds = 2.80;
constexpr double kThrowIntervalPhase3Seconds = 2.40;
constexpr double kMeleeIntervalSeconds = 4.50;

constexpr double kDefaultDodgeChance = 0.55;

BossPhase phaseForHp(int hp) {
    if (hp <= 0) {
        return BossPhase::Dead;
    }
    if (hp == 1) {
        return BossPhase::Phase3;
    }
    if (hp <= 3) {
        return BossPhase::Phase2;
    }
    return BossPhase::Phase1;
}
}

GorillaBoss::GorillaBoss(double x, double y, unsigned seed)
    : Enemy(x, y, kBossWidth, kBossHeight),
      currentHp(kMaxHp),
      phase(BossPhase::Phase1),
      announcedPhase(BossPhase::Phase1),
      action(BossAction::Idle),
      stage(BossAttackStage::Prepare),
      actionElapsedSeconds(0.0),
      actionDurationSeconds(kIdleTickSeconds),
      throwCooldownSeconds(kThrowIntervalPhase1Seconds),
      meleeCooldownSeconds(kMeleeIntervalSeconds),
      hurtWindowSeconds(0.0),
      dodgeCooldownSeconds(0.0),
      deathElapsedSeconds(0.0),
      dodgeChance(kDefaultDodgeChance),
      playerCenterX(x),
      movementMinX(0.0),
      movementMaxX(0.0),
      consecutiveDodges(0),
      pendingThrow(false),
      pendingSlamImpact(false),
      pendingDodgeCue(false),
      pendingDeathCue(false),
      deathStarted(false),
      deathFinished(false),
      nextMeleeIsCharge(true),
      randomEngine(seed) {
    direction = Direction::Left;
}

int GorillaBoss::getCurrentHp() const {
    return currentHp;
}

int GorillaBoss::getMaxHp() const {
    return kMaxHp;
}

BossPhase GorillaBoss::getPhase() const {
    return phase;
}

BossAction GorillaBoss::getAction() const {
    return action;
}

BossAttackStage GorillaBoss::getAttackStage() const {
    return stage;
}

bool GorillaBoss::isAttackActive() const {
    return stage == BossAttackStage::Active &&
           (action == BossAction::Charge || action == BossAction::GroundSlam);
}

bool GorillaBoss::isDeathStarted() const {
    return deathStarted;
}

bool GorillaBoss::isDeathFinished() const {
    return deathFinished;
}

int GorillaBoss::getConsecutiveDodges() const {
    return consecutiveDodges;
}

Direction GorillaBoss::getThrowDirection() const {
    return direction;
}

bool GorillaBoss::consumeThrowRequest() {
    if (!pendingThrow) {
        return false;
    }

    pendingThrow = false;
    return true;
}

bool GorillaBoss::consumeSlamImpactCue() {
    if (!pendingSlamImpact) {
        return false;
    }

    pendingSlamImpact = false;
    return true;
}

bool GorillaBoss::consumeDodgeCue() {
    if (!pendingDodgeCue) {
        return false;
    }

    pendingDodgeCue = false;
    return true;
}

bool GorillaBoss::consumeDeathCue() {
    if (!pendingDeathCue) {
        return false;
    }

    pendingDeathCue = false;
    return true;
}

void GorillaBoss::setMovementRegion(double minX, double maxX) {
    movementMinX = std::min(minX, maxX);
    movementMaxX = std::max(minX, maxX);
    clampToRegion();
}

void GorillaBoss::trackPlayerCenter(double centerX) {
    playerCenterX = centerX;
}

void GorillaBoss::setDodgeChance(double chance) {
    dodgeChance = std::clamp(chance, 0.0, 1.0);
}

double GorillaBoss::getDodgeChance() const {
    return dodgeChance;
}

// Mỗi shell chỉ được quyết định một lần: hoặc trúng, hoặc bị né hẳn.
BossHitResult GorillaBoss::onShellHit(const Koopa& shell) {
    if (deathStarted || phase == BossPhase::Dead) {
        return BossHitResult::Ignored;
    }

    const std::uint32_t shellId = shell.getId();
    if (decidedShellIds.find(shellId) != decidedShellIds.end()) {
        return BossHitResult::Ignored;
    }

    // Cửa sổ hurt chặn multi-hit từ nhiều shell trong cùng một nhịp.
    if (hurtWindowSeconds > 0.0) {
        return BossHitResult::Ignored;
    }

    decidedShellIds.insert(shellId);

    if (canDodge() && rollDodge()) {
        ++consecutiveDodges;
        dodgeCooldownSeconds = kDodgeCooldownSeconds;
        pendingDodgeCue = true;
        startAction(BossAction::Dodge, BossAttackStage::Active, kDodgeSeconds);
        return BossHitResult::Dodged;
    }

    consecutiveDodges = 0;
    applyShellDamage();
    return BossHitResult::Damaged;
}

bool GorillaBoss::canDodge() const {
    return phase == BossPhase::Phase3 && dodgeCooldownSeconds <= 0.0 &&
           consecutiveDodges < kMaxConsecutiveDodges;
}

bool GorillaBoss::rollDodge() {
    std::uniform_real_distribution<double> roll(0.0, 1.0);
    return roll(randomEngine) < dodgeChance;
}

void GorillaBoss::applyShellDamage() {
    currentHp = std::max(0, currentHp - 1);
    hurtWindowSeconds = kHurtSeconds;

    if (currentHp == 0) {
        refreshPhase();
        startDeath();
        return;
    }

    const BossPhase previousPhase = announcedPhase;
    refreshPhase();

    if (phase != previousPhase) {
        // Chuyển phase chỉ phát cue Enraged đúng một lần tại mỗi ngưỡng.
        startAction(BossAction::Enraged, BossAttackStage::Active,
                    kEnragedSeconds);
        return;
    }

    startAction(BossAction::Hurt, BossAttackStage::Active, kHurtSeconds);
}

void GorillaBoss::refreshPhase() {
    phase = phaseForHp(currentHp);
    announcedPhase = phase;
}

void GorillaBoss::startDeath() {
    if (deathStarted) {
        return;
    }

    deathStarted = true;
    deathElapsedSeconds = 0.0;
    pendingThrow = false;
    pendingSlamImpact = false;
    pendingDeathCue = true;
    startAction(BossAction::Death, BossAttackStage::Recover, kDeathSeconds);
    Enemy::die();
}

void GorillaBoss::startAction(BossAction nextAction, BossAttackStage nextStage,
                              double durationSeconds) {
    action = nextAction;
    stage = nextStage;
    actionElapsedSeconds = 0.0;
    actionDurationSeconds = std::max(0.01, durationSeconds);
}

void GorillaBoss::advanceAction() {
    switch (action) {
        case BossAction::WindUpThrow:
            pendingThrow = true;
            startAction(BossAction::Throw, BossAttackStage::Active,
                        kThrowSeconds);
            return;
        case BossAction::Throw:
            startAction(BossAction::Recover, BossAttackStage::Recover,
                        kRecoverSeconds);
            return;
        case BossAction::Charge:
            if (stage == BossAttackStage::Prepare) {
                startAction(BossAction::Charge, BossAttackStage::Active,
                            kChargeActiveSeconds);
            } else if (stage == BossAttackStage::Active) {
                startAction(BossAction::Charge, BossAttackStage::Recover,
                            kChargeRecoverSeconds);
            } else {
                beginNextDecision();
            }
            return;
        case BossAction::GroundSlam:
            if (stage == BossAttackStage::Prepare) {
                pendingSlamImpact = true;
                startAction(BossAction::GroundSlam, BossAttackStage::Active,
                            kSlamActiveSeconds);
            } else if (stage == BossAttackStage::Active) {
                startAction(BossAction::GroundSlam, BossAttackStage::Recover,
                            kSlamRecoverSeconds);
            } else {
                beginNextDecision();
            }
            return;
        case BossAction::Death:
            return;
        case BossAction::Idle:
        case BossAction::Recover:
        case BossAction::Hurt:
        case BossAction::Enraged:
        case BossAction::Dodge:
        default:
            beginNextDecision();
            return;
    }
}

// Phase 1 chỉ ném; từ Phase 2 xen kẽ Charge và GroundSlam, luôn có recovery.
void GorillaBoss::beginNextDecision() {
    if (phase == BossPhase::Dead || deathStarted) {
        return;
    }

    if (phase != BossPhase::Phase1 && meleeCooldownSeconds <= 0.0) {
        meleeCooldownSeconds = kMeleeIntervalSeconds;
        if (nextMeleeIsCharge) {
            nextMeleeIsCharge = false;
            startAction(BossAction::Charge, BossAttackStage::Prepare,
                        kChargePrepareSeconds);
        } else {
            nextMeleeIsCharge = true;
            startAction(BossAction::GroundSlam, BossAttackStage::Prepare,
                        kSlamPrepareSeconds);
        }
        return;
    }

    if (throwCooldownSeconds <= 0.0) {
        switch (phase) {
            case BossPhase::Phase3:
                throwCooldownSeconds = kThrowIntervalPhase3Seconds;
                break;
            case BossPhase::Phase2:
                throwCooldownSeconds = kThrowIntervalPhase2Seconds;
                break;
            case BossPhase::Phase1:
            default:
                throwCooldownSeconds = kThrowIntervalPhase1Seconds;
                break;
        }
        startAction(BossAction::WindUpThrow, BossAttackStage::Prepare,
                    kWindUpSeconds);
        return;
    }

    startAction(BossAction::Idle, BossAttackStage::Prepare, kIdleTickSeconds);
}

void GorillaBoss::faceTowardsPlayer() {
    // Đang lao thì giữ nguyên hướng, nếu không cú charge sẽ bám dính Mario.
    if (action == BossAction::Charge && stage != BossAttackStage::Prepare) {
        return;
    }

    const double bossCenterX = x + width / 2.0;
    direction = (playerCenterX < bossCenterX) ? Direction::Left
                                              : Direction::Right;
}

void GorillaBoss::clampToRegion() {
    if (movementMaxX <= movementMinX) {
        return;
    }

    x = std::clamp(x, movementMinX, movementMaxX - width);
}

void GorillaBoss::patrol() {
    if (deathStarted || !alive) {
        velocityX = 0.0;
        return;
    }

    if (action != BossAction::Charge || stage != BossAttackStage::Active) {
        velocityX = 0.0;
        return;
    }

    const double speed = (direction == Direction::Left)
                             ? -kChargeSpeedPixelsPerSecond
                             : kChargeSpeedPixelsPerSecond;

    // Không lao ra ngoài vùng arena mà C giao.
    if (movementMaxX > movementMinX) {
        if (speed < 0.0 && x <= movementMinX) {
            velocityX = 0.0;
            return;
        }
        if (speed > 0.0 && x + width >= movementMaxX) {
            velocityX = 0.0;
            return;
        }
    }

    velocityX = speed;
}

void GorillaBoss::update(double dtSeconds) {
    if (deathStarted) {
        velocityX = 0.0;
        deathElapsedSeconds += dtSeconds;
        if (deathElapsedSeconds >= kDeathSeconds) {
            deathFinished = true;
        }
        return;
    }

    hurtWindowSeconds = std::max(0.0, hurtWindowSeconds - dtSeconds);
    dodgeCooldownSeconds = std::max(0.0, dodgeCooldownSeconds - dtSeconds);
    throwCooldownSeconds = std::max(0.0, throwCooldownSeconds - dtSeconds);
    meleeCooldownSeconds = std::max(0.0, meleeCooldownSeconds - dtSeconds);

    actionElapsedSeconds += dtSeconds;
    if (actionElapsedSeconds >= actionDurationSeconds) {
        advanceAction();
    }

    faceTowardsPlayer();
    applyGravity(dtSeconds);
    patrol();
    clampToRegion();
}

// Boss chỉ chết khi HP về 0; fireball hay shell lẻ không được gọi thẳng die().
void GorillaBoss::die() {}

void GorillaBoss::onPlayerContact(Player& player) {
    if (deathStarted) {
        return;
    }

    Enemy::onPlayerContact(player);
}

void GorillaBoss::onStomped(Player& player) {
    // Dẫm lên boss không gây damage, chỉ là một cú chạm bình thường.
    onPlayerContact(player);
}

bool GorillaBoss::isStompable() const {
    return false;
}

bool GorillaBoss::shouldTurnAtEdge() const {
    return false;
}

bool GorillaBoss::takesFireballDamage() const {
    return false;
}

// Xác boss ở lại arena cho tới khi load lại level, C cần nó để chạy death.
bool GorillaBoss::isRemovable() const {
    return false;
}
