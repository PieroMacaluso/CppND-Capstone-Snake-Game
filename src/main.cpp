#include <iostream>
#include <fstream>
#include <string>
#include "controller.h"
#include "game.h"
#include "renderer.h"

int main(int argc, char* argv[])
{
  constexpr std::size_t kFramesPerSecond{60};
  constexpr std::size_t kMsPerFrame{1000 / kFramesPerSecond};
  constexpr std::size_t kScreenWidth{640};
  constexpr std::size_t kScreenHeight{640};
  constexpr std::size_t kGridWidth{32};
  constexpr std::size_t kGridHeight{32};
  bool continue_flag = true;

  // Map name extraction and check
  std::string map_file = "map_1.txt";
  if (argc == 2)
    map_file = std::string(argv[1]);
  std::ifstream file(map_file);
  if (!file.good()) {
    std::cout << "Map file not found. Exiting.\n";
    return 0;
  }

  // Main Loop
  while (continue_flag)
  {
    Renderer renderer(kScreenWidth, kScreenHeight, kGridWidth, kGridHeight);
    Controller controller;
    Game game(kGridWidth, kGridHeight, map_file);
    game.Run(controller, renderer, kMsPerFrame);
    std::cout << "Game has terminated successfully!\n";
    std::cout << "Score: " << game.GetScore() << "\n";
    std::cout << "Size: " << game.GetSize() << "\n";
    std::cout << "Press SPACE to restart or ESC to exit\n";
    continue_flag = controller.HandleEndInput();
  }
  return 0;
}