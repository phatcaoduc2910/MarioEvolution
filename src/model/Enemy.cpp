#include "model/Enemy.h"

#include "model/Player.h"

namespace {
constexpr double kWalkingSpeedPixelsPerSecond = 60.0;
constexpr double kShellSlideSpeedPixelsPerSecond = 300.0;

constexpr double kPiranhaHiddenSeconds = 1.8;
constexpr double kPiranhaRisingSeconds = 0.45;
constexpr double kPiranhaExposedSeconds = 1.6;
constexpr double kPiranhaSinkingSeconds = 0.45;
constexpr double kEnemyDeathDisplaySeconds = 0.5;

Direction awayFromPlayer(const Player& player, const GameObject& enemy) {
    const double playerCenterX = player.getX() + player.getWidth() / 2.0;
    const double enemyCenterX = enemy.getX() + enemy.getWidth() / 2.0;

    return (playerCenterX <= enemyCenterX) ? Direction::Right : Direction::Left;
}

double phaseDurationSeconds(PiranhaPhase phase) {
    switch (phase) {
        case PiranhaPhase::Hidden:
            return kPiranhaHiddenSeconds;
        case PiranhaPhase::Rising:
            return kPiranhaRisingSeconds;
        case PiranhaPhase::Exposed:
            return kPiranhaExposedSeconds;
        case PiranhaPhase::Sinking:
        default:
            return kPiranhaSinkingSeconds;
    }
}

PiranhaPhase nextPhase(PiranhaPhase phase) {
    switch (phase) {
        case PiranhaPhase::Hidden:
            return PiranhaPhase::Rising;
        case PiranhaPhase::Rising:
            return PiranhaPhase::Exposed;
        case PiranhaPhase::Exposed:
            return PiranhaPhase::Sinking;
        case PiranhaPhase::Sinking:
        default:
            return PiranhaPhase::Hidden;
    }
}

double risenRatio(PiranhaPhase phase, double phaseElapsedSeconds) {
    switch (phase) {
        case PiranhaPhase::Hidden:
            return 0.0;
        case PiranhaPhase::Rising:
            return phaseElapsedSeconds / kPiranhaRisingSeconds;
        case PiranhaPhase::Exposed:
            return 1.0;
        case PiranhaPhase::Sinking:
        default:
            return 1.0 - phaseElapsedSeconds / kPiranhaSinkingSeconds;
    }
}
}

Enemy::Enemy(double x, double y, int width, int height)
    : Actor(x, y, width, height),
      walkingSpeed(kWalkingSpeedPixelsPerSecond),
      state(EnemyState::Walking),
      deathElapsedSeconds(0.0) {}

void Enemy::patrol() {
    if (!alive || state == EnemyState::Dead) {
        return;
    }

    velocityX = (direction == Direction::Left) ? -walkingSpeed : walkingSpeed;
}

void Enemy::reverseDirection() {
    direction = (direction == Direction::Left) ? Direction::Right : Direction::Left;
}

bool Enemy::shouldTurnAtEdge() const {
    return true;
}

bool Enemy::isStompable() const {
    return true;
}

void Enemy::onStomped(Player& player) {
    (void)player;
    die();
}

bool Enemy::isDeadlyToEnemies() const {
    return false;
}

bool Enemy::takesFireballDamage() const {
    return true;
}

bool Enemy::isRemovable() const {
    return !alive && deathElapsedSeconds >= kEnemyDeathDisplaySeconds;
}

void Enemy::tickDeath(double dtSeconds) {
    deathElapsedSeconds += dtSeconds;
}

void Enemy::update(double dtSeconds) {
    if (!alive) {
        tickDeath(dtSeconds);
        return;
    }

    applyGravity(dtSeconds);
    patrol();
}

void Enemy::die() {
    if (!alive) {
        return;
    }

    state = EnemyState::Dead;
    alive = false;
    velocityX = 0.0;
    velocityY = 0.0;
    deathElapsedSeconds = 0.0;
}

void Enemy::onPlayerContact(Player& player) {
    if (alive && state != EnemyState::Dead) {
        player.takeDamage();
    }
}

Goomba::Goomba(double x, double y)
    : Enemy(x, y, 32, 32) {
    direction = Direction::Left;
}

void Goomba::patrol() {
    Enemy::patrol();
}

void Goomba::die() {
    Enemy::die();
}

Koopa::Koopa(double x, double y, KoopaColor color)
    : Enemy(x, y, kWalkWidth, kWalkHeight),
      color(color),
      thrown(false) {
    direction = Direction::Left;
}

KoopaColor Koopa::getColor() const {
    return color;
}

bool Koopa::isShell() const {
    return state == EnemyState::Shell;
}

bool Koopa::isSlidingShell() const {
    return state == EnemyState::ShellSliding;
}

void Koopa::hideInShell() {
    if (!alive) {
        return;
    }

    if (height > kShellHeight) {
        y += height - kShellHeight;
        height = kShellHeight;
    }

    state = EnemyState::Shell;
    velocityX = 0.0;
}

// Koopa do boss ném hoặc thả từ trên: quỹ đạo giữ nguyên tới khi chạm đất,
// sau đó nó quay lại là một Koopa bình thường để Mario đạp thành mai.
void Koopa::throwWith(double throwVelocityX, double throwVelocityY) {
    if (!alive || state == EnemyState::Dead) {
        return;
    }

    thrown = true;
    onGround = false;
    velocityX = throwVelocityX;
    velocityY = throwVelocityY;
    direction = (throwVelocityX < 0.0) ? Direction::Left : Direction::Right;
}

bool Koopa::isAirborne() const {
    return thrown;
}

void Koopa::kick(Direction slideDirection) {
    if (!alive || state == EnemyState::Dead) {
        return;
    }

    direction = slideDirection;
    state = EnemyState::ShellSliding;
}

void Koopa::patrol() {
    if (!alive || state == EnemyState::Dead) {
        return;
    }

    if (thrown) {
        if (!onGround) {
            // Giữ vận tốc ngang của cú ném, patrol thường sẽ ghi đè mất.
            return;
        }
        thrown = false;
    }

    if (state == EnemyState::Shell) {
        velocityX = 0.0;
        return;
    }

    const double speed = (state == EnemyState::ShellSliding)
                             ? kShellSlideSpeedPixelsPerSecond
                             : walkingSpeed;
    velocityX = (direction == Direction::Left) ? -speed : speed;
}

// Chạm ngang: Walking và ShellSliding đều trừ máu, Shell đứng yên thì bị đá đi.
void Koopa::onPlayerContact(Player& player) {
    if (state != EnemyState::Shell) {
        // Enemy::onPlayerContact đã bỏ qua sẵn trạng thái Dead.
        Enemy::onPlayerContact(player);
        return;
    }

    const Direction slideDirection = awayFromPlayer(player, *this);
    kick(slideDirection);

    // Tách shell khỏi hộp player ngay trong frame đá: còn chồng thì frame sau
    // đọc thành cú chạm ngang với shell đang chạy và trừ máu oan.
    const Rectangle playerBounds = player.getBounds();
    placeBesideWall(
        (slideDirection == Direction::Right)
            ? playerBounds.x + playerBounds.width
            : playerBounds.x - width);
}

// Dẫm từ trên: Walking thu vào mai, ShellSliding dừng lại, Shell đứng yên giữ
// nguyên (player chỉ bật lên, phần bật do CollisionSystem lo).
void Koopa::onStomped(Player& player) {
    (void)player;
    if (!alive || state == EnemyState::Dead || state == EnemyState::Shell) {
        return;
    }

    hideInShell();
}

bool Koopa::shouldTurnAtEdge() const {
    return color == KoopaColor::Red && state == EnemyState::Walking;
}

bool Koopa::isDeadlyToEnemies() const {
    return alive && state == EnemyState::ShellSliding;
}

PiranhaPlant::PiranhaPlant(double x, double mouthY)
    : Enemy(x, mouthY, kPlantWidth, 0),
      phase(PiranhaPhase::Hidden),
      phaseElapsedSeconds(0.0),
      mouthY(mouthY),
      oneShot(false),
      hasRisen(false) {}

PiranhaPhase PiranhaPlant::getPhase() const {
    return phase;
}

// Arena bật hazard: cây chạy Hidden (warning) -> Rising -> Exposed -> Sinking
// đúng một vòng rồi tự chết để scheduler không phải dọn tay.
void PiranhaPlant::activateOnce() {
    oneShot = true;
    hasRisen = false;
    phase = PiranhaPhase::Hidden;
    phaseElapsedSeconds = 0.0;
}

bool PiranhaPlant::isOneShot() const {
    return oneShot;
}

void PiranhaPlant::update(double dtSeconds) {
    if (!alive) {
        tickDeath(dtSeconds);
        return;
    }

    phaseElapsedSeconds += dtSeconds;
    while (phaseElapsedSeconds >= phaseDurationSeconds(phase)) {
        phaseElapsedSeconds -= phaseDurationSeconds(phase);
        phase = nextPhase(phase);
        if (phase == PiranhaPhase::Rising) {
            hasRisen = true;
        }
    }

    applyRisenHeight();

    if (oneShot && hasRisen && phase == PiranhaPhase::Hidden) {
        die();
    }
}

void PiranhaPlant::applyGravity(double) {}

void PiranhaPlant::patrol() {}

void PiranhaPlant::onPlayerContact(Player& player) {
    if (phase == PiranhaPhase::Hidden) {
        return;
    }

    Enemy::onPlayerContact(player);
}

bool PiranhaPlant::isStompable() const {
    return false;
}

bool PiranhaPlant::shouldTurnAtEdge() const {
    return false;
}

void PiranhaPlant::applyRisenHeight() {
    const double ratio = risenRatio(phase, phaseElapsedSeconds);
    height = static_cast<int>(kPlantHeight * ratio);
    y = mouthY - height;
}
