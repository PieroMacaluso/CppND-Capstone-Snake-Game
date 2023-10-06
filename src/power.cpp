#include "power.h"

void SpeedupPower::apply(Snake &snake)
{
    snake.speed += 0.02;
}
void SlowdownPower::apply(Snake &snake)
{
    snake.speed -= 0.02;
}