#ifndef EFFECT_H
#define EFFECT_H

#include <string>
#include <memory>

class Enemy;

class Effect
{
protected:
    std::string name_;
    float duration_; // Total lifetime of the effect. 0 means instant effect.
    float timer_; 
    float tickInterval_; // Interval between ticks. 0 means it ticks every frame.
    float timeSinceLastTick_; 
    bool applied_once_;

public:
    Effect(const std::string& name, float duration, float tickInterval = 0.0f);
    virtual ~Effect();

public:

    /**
     * @brief Call at each frame to update effects on enemy
     * 
     * @param target 
     * @param deltaTime
     */
    void apply(Enemy& target, float deltaTime);

    virtual void onStart(Enemy& target) {}
    virtual void onTick(Enemy& target, float deltaTime) {}
    virtual void onEnd(Enemy& target) {}

    bool isExpired() const;
    bool isInstant() const;
    const std::string& getName() const;
    
    /**
     * @brief Resets the timer of the effect (e.g. when an enemy is hit again to refresh the duration)
     */
    virtual void resetTimer() { timer_ = 0.0f; }
};

#endif