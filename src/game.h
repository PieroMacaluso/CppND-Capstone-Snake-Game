#ifndef GAME_H
#define GAME_H

#include <random>
#include <string>
#include <mutex>
#include <thread>
#include <memory>
#include <condition_variable>
#include "SDL.h"
#include "controller.h"
#include "renderer.h"
#include "snake.h"
#include "power.h"

class Game {
 public:
  Game(std::size_t grid_width, std::size_t grid_height, std::string obstacle_file_path);
  void Run(Controller const &controller, Renderer &renderer,
           std::size_t target_frame_duration);
  int GetScore() const;
  int GetSize() const;
  bool checkPower(PowerType type);

 private:
  Snake snake;
  SDL_Point food;
  std::shared_ptr<Power> power;
  bool power_visible = false;
  std::vector<std::vector<bool>> obstacles;

  std::random_device dev;
  std::mt19937 engine;
  std::uniform_int_distribution<int> random_w;
  std::uniform_int_distribution<int> random_h;
  std::uniform_int_distribution<int> random_t;
  std::mutex mutex;
  std::condition_variable cv;
  std::thread speedup_th;
  std::thread slowdown_th;


  int score{0};

  void PlaceFood();
  void PlaceSpeedup();
  void PlaceSlowdown();
  void PlaceObstacles(std::string obstacles_path);
  void Update();
};

#endif