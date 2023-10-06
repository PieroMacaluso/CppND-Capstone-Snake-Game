#ifndef POWER_H
#define POWER_H

#include "snake.h"

enum class PowerType
{
    kSpeedup,
    kSlowdown
};

class Power
{
public:
    Power(PowerType type, int x, int y) : type(type), x(x), y(y) {}
    PowerType type;
    int x;
    int y;
    virtual void apply(Snake &snake) = 0;
private:
};

class SpeedupPower : public Power
{
public:
    SpeedupPower(int x, int y) : Power(PowerType::kSpeedup, x, y) {}
    void apply(Snake &snake);

private:
};

class SlowdownPower : public Power
{
public:
    SlowdownPower(int x, int y) : Power(PowerType::kSlowdown, x, y) {}
    void apply(Snake &snake);
private:
};

#endif