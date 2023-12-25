#ifndef GAME_H
#define GAME_H

#include <random>
#include "SDL.h"
#include "controller.h"
#include "renderer.h"
#include "snake.h"

class Game {
 public:
  Game(std::size_t grid_width, std::size_t grid_height);
  void Run(Controller const &controller, Renderer *renderer,
           std::size_t target_frame_duration);
  int GetScore() const;
  int GetSize() const;

  // to toggle game pause
  void ToggleGamePause();

  friend class Controller;

 private:
  Snake snake;
  SDL_Point food;

  std::random_device dev;
  std::mt19937 engine;
  std::uniform_int_distribution<int> random_w;
  std::uniform_int_distribution<int> random_h;

  // game pause and resume
  bool _paused;
  bool _wallUp;

  int score{0};

  void PlaceFood();
  void Update(Renderer *renderer);
  void Pause();
  void Resume();
};

#endif
