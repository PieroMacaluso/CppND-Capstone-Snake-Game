#include "game.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include "SDL.h"

Game::Game(std::size_t grid_width, std::size_t grid_height, std::string obstacle_file_path)
  : snake(grid_width, grid_height),
  engine(dev()),
  random_w(0, static_cast<int>(grid_width - 1)),
  random_h(0, static_cast<int>(grid_height - 1)),
  random_t(2, 5)
{
  PlaceObstacles(obstacle_file_path);
  PlaceFood();
  speedup_th = std::thread(&Game::PlaceSpeedup, this);
  slowdown_th = std::thread(&Game::PlaceSlowdown, this);
}

void Game::Run(Controller const& controller, Renderer& renderer,
  std::size_t target_frame_duration)
{
  Uint32 title_timestamp = SDL_GetTicks();
  Uint32 frame_start;
  Uint32 frame_end;
  Uint32 frame_duration;
  int frame_count = 0;
  bool running = true;

  while (running)
  {
    frame_start = SDL_GetTicks();

    // Input, Update, Render - the main game loop.
    controller.HandleInput(running, snake);
    Update();
    renderer.Render(snake, food, obstacles, power);

    frame_end = SDL_GetTicks();

    // Keep track of how long each loop through the input/update/render cycle
    // takes.
    frame_count++;
    frame_duration = frame_end - frame_start;

    // After every second, update the window title.
    if (frame_end - title_timestamp >= 1000)
    {
      renderer.UpdateWindowTitle(score, frame_count);
      frame_count = 0;
      title_timestamp = frame_end;
    }

    // If the time for this frame is too small (i.e. frame_duration is
    // smaller than the target ms_per_frame), delay the loop to
    // achieve the correct frame rate.
    running = snake.alive;
    if (frame_duration < target_frame_duration)
    {
      SDL_Delay(target_frame_duration - frame_duration);
    }
  }
  slowdown_th.join();
  speedup_th.join();
}

void Game::PlaceFood()
{
  int x, y;
  while (true)
  {
    x = random_w(engine);
    y = random_h(engine);
    // Check that the location is not occupied by a snake item before placing
    // food.
    if (!snake.SnakeCell(x, y) && !obstacles[x][y])
    {
      food.x = x;
      food.y = y;
      return;
    }
  }
}

void Game::PlaceObstacles(std::string obstacles_path)
{
  std::cout << std::filesystem::current_path() << std::endl;
  // Read obstacles from file
  std::ifstream obstacles_file(obstacles_path);
  std::string line;
  while (std::getline(obstacles_file, line))
  {
    std::vector<bool> obstacle;
    for (char c : line)
    {
      obstacle.push_back(c == '1');
    }
    obstacles.push_back(obstacle);
  }
}

void Game::PlaceSpeedup()
{
  std::this_thread::sleep_for(std::chrono::seconds(random_t(engine)));
  while (snake.alive)
  {
    std::unique_lock lk(mutex);
    cv.wait(lk, [this]
      { return this->checkPower(PowerType::kSlowdown); });
    int x, y;
    while (true)
    {
      x = random_w(engine);
      y = random_h(engine);
      if (!snake.SnakeCell(x, y) && !obstacles[x][y] && food.x != x && food.y != y)
      {
        power = std::make_shared<SpeedupPower>(x, y);
        break;
      }
    }
    std::this_thread::sleep_for(std::chrono::seconds(random_t(engine)));
    lk.unlock();
    cv.notify_one();
  }
}

void Game::PlaceSlowdown()
{
  std::this_thread::sleep_for(std::chrono::seconds(random_t(engine)));
  while (snake.alive)
  {
    std::unique_lock lk(mutex);
    cv.wait(lk, [this]
      { return this->checkPower(PowerType::kSpeedup); });
    int x, y;
    while (true)
    {
      x = random_w(engine);
      y = random_h(engine);
      if (!snake.SnakeCell(x, y) && !obstacles[x][y] && food.x != x && food.y != y)
      {
        power = std::make_shared<SlowdownPower>(x, y);
        break;
      }
    }
    std::this_thread::sleep_for(std::chrono::seconds(random_t(engine)));
    lk.unlock();
    cv.notify_one();
  }
}

void Game::Update()
{
  if (!snake.alive)
    return;

  snake.Update();

  // Check if snake is in an obstacle
  int x = static_cast<int>(snake.head_x);
  int y = static_cast<int>(snake.head_y);
  if (obstacles[x][y])
  {
    snake.alive = false;
  }

  int new_x = static_cast<int>(snake.head_x);
  int new_y = static_cast<int>(snake.head_y);

  // Check if there's food over here
  if (food.x == new_x && food.y == new_y)
  {
    score++;
    PlaceFood();
    // Grow snake and increase speed.
    snake.GrowBody();
    snake.speed += 0.02;
  }
  // Check if there's powerup over here
  if (power != nullptr && power->x == new_x && power->y == new_y)
  {
    std::cout << "Powerup taken" << std::endl;
    power->apply(snake);
    power.reset();
  }
}

int Game::GetScore() const { return score; }
int Game::GetSize() const { return snake.size; }

bool Game::checkPower(PowerType type)
{
  if (power == nullptr)
  {
    return true;
  }
  if (power != nullptr && power->type == type)
  {
    return true;
  }
  return false;
}