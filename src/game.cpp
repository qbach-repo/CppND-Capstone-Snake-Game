#include "game.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <future>
#include "SDL.h"
#include "snake.h"

Game::Game(std::size_t grid_width, std::size_t grid_height)
    : snake(grid_width, grid_height),
      engine(dev()),
      random_w(0, static_cast<int>(grid_width - 1)),
      random_h(0, static_cast<int>(grid_height - 1)) {
  _paused = false;
  _wallUp = false;
  PlaceFood();
}

void TimerThread(Snake &snake) {
  auto old_speed = snake.speed;
  snake.speed *= 2.0;
  std::this_thread::sleep_for(std::chrono::seconds(5));
  snake.speed = old_speed + 0.02;
}

void Game::Run(Controller const &controller, Renderer *renderer,
               std::size_t target_frame_duration) {
  Uint32 title_timestamp = SDL_GetTicks();
  Uint32 frame_start;
  Uint32 frame_end;
  Uint32 frame_duration;
  int frame_count = 0;
  bool running = true;

  while (running) {
    frame_start = SDL_GetTicks();

    // Input, Update, Render - the main game loop.
    controller.HandleInput(running, snake, *this);
    Update(renderer);
    renderer->Render(snake, food, &_wallUp);

    frame_end = SDL_GetTicks();

    // Keep track of how long each loop through the input/update/render cycle
    // takes.
    frame_count++;
    frame_duration = frame_end - frame_start;

    // After every second, update the window title.
    if (frame_end - title_timestamp >= 1000) {
      renderer->UpdateWindowTitle(score, frame_count);
      frame_count = 0;
      title_timestamp = frame_end;
    }

    // If the time for this frame is too small (i.e. frame_duration is
    // smaller than the target ms_per_frame), delay the loop to
    // achieve the correct frame rate.
    if (frame_duration < target_frame_duration) {
      SDL_Delay(target_frame_duration - frame_duration);
    }
  }
}

void Game::PlaceFood() {
  int x, y;
  while (true) {
    x = random_w(engine);
    y = random_h(engine);
    // Check that the location is not occupied by a snake item before placing
    // food.
    if (!snake.SnakeCell(x, y)) {
      food.x = x;
      food.y = y;
      return;
    }
  }
}

void Game::Update(Renderer *renderer) {
  if (this->_paused == true) {
    renderer->SetPauseScreen();
    return;
  }
  if (!snake.alive) return;

  snake.Update(&_wallUp);

  int new_x = static_cast<int>(snake.head_x);
  int new_y = static_cast<int>(snake.head_y);

  std::random_device rd;  //Will be used to obtain a seed for the random number engine
  std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
  std::uniform_int_distribution<> dis(1, 10);

  // Check if there's food over here
  if (food.x == new_x && food.y == new_y) {
    score++;

    PlaceFood();
    // Grow snake and increase speed.
    snake.GrowBody();
    if (dis(gen) <= 2) {
      std::thread hyperTimer(TimerThread, snake);
      hyperTimer.detach();
    } else {
      snake.speed += 0.02;
    }

  }
}

int Game::GetScore() const { return score; }
int Game::GetSize() const { return snake.size; }


void Game::ToggleGamePause() {
  this->_paused ? Resume() : Pause();
}

void Game::Pause() {
  this->_paused = true;
}

void Game::Resume() {
  this->_paused = false;
}
