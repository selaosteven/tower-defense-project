#include "Entities/Effect.h"
#include "Entities/Enemy.h"

Effect::Effect(const std::string& name, float duration, float tickInterval) 
    : name_{name}, duration_{duration}, timer_{0.0f}, tickInterval_{tickInterval}, 
      timeSinceLastTick_{0.0f}, applied_once_{false} 
{
}

Effect::~Effect() {}

void Effect::apply(Enemy& target, float deltaTime) {
    if (isExpired()) return;

    // initialization 
    if (!applied_once_) {
        onStart(target);
        applied_once_ = true;
    }

    // Instant effects trigger once and then expire immediately
    if (isInstant()) {
        onTick(target, deltaTime);
        timer_ = 1.0f; // Force expiration for instant effects
    } else {
        if (tickInterval_ > 0.0f) {
            timeSinceLastTick_ += deltaTime;
            while (timeSinceLastTick_ >= tickInterval_) {
                onTick(target, tickInterval_);
                timeSinceLastTick_ -= tickInterval_; // Catch up correctly
            }
        } else {
            // Effects with a duration tick over time every frame
            onTick(target, deltaTime);
        }
        timer_ += deltaTime;
        if (isExpired()) {
            onEnd(target);
        }
    }
}

bool Effect::isExpired() const {
    if (isInstant()) return applied_once_;
    return timer_ >= duration_;
}

bool Effect::isInstant() const {
    return duration_ <= 0.0f;
}

const std::string& Effect::getName() const {
    return name_;
}
