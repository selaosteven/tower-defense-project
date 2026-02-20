#ifndef EFFECT_H
#define EFFECT_H
class Effect
{
private:
    /* data */
public:
    Effect(/* args */);
    ~Effect();

// methods

public:
    void apply(Enemy& target);
};

#endif