#include "main.h"
#include "car.h"
#include <SDL2/SDL.h>

Car::Car(int start_x, int start_y, int car_side_speed) {
  x = start_x;
  y = start_y;
  width = CAR_WIDTH;
  height = CAR_HEIGHT;
  speed_sideways = car_side_speed;
}

void Car::move_sideways(const Uint8* keystates) {
  const int track_left = SCREEN_WIDTH / 4;
  const int track_right = SCREEN_WIDTH * 3 / 4;

  if (keystates[SDL_SCANCODE_A] || keystates[SDL_SCANCODE_LEFT]) {
    x -= speed_sideways;
  }
  if (keystates[SDL_SCANCODE_D] || keystates[SDL_SCANCODE_RIGHT]) {
    x += speed_sideways;
  }

  if (x < track_left) {
    x = track_left;
  }

  if (x + width > track_right) {
    x = track_right - width;
  }
}

void Car::render(SDL_Renderer* rend) {
  SDL_Rect car_body;
  car_body.x = x;
  car_body.y = y;
  car_body.w = width;
  car_body.h = height;

  SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
  SDL_RenderFillRect(rend, &car_body);

  SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);

  SDL_Rect wheel;

  wheel.w = wheel_width;
  wheel.h = wheel_height;
  wheel.x = x - wheel_width;
  wheel.y = y + height / 6;
  SDL_RenderFillRect(rend, &wheel);

  wheel.x = x + width;
  SDL_RenderFillRect(rend, &wheel);

  wheel.x = x - wheel_width;
  wheel.y = y + height - wheel_height - 5;
  SDL_RenderFillRect(rend, &wheel);

  wheel.x = x + width;
  SDL_RenderFillRect(rend, &wheel);
}

int Car::get_x() const { return x; }
void Car::set_x(int new_x) { x = new_x; }

int Car::get_y() const { return y; }
void Car::set_y(int new_y) { y = new_y; }

int Car::get_width() const { return width; }
void Car::set_width(int new_width) { width = new_width; }

int Car::get_height() const { return height; }
void Car::set_height(int new_height) { height = new_height; }

int Car::get_speed_sideways() const { return speed_sideways; }
void Car::set_speed_sideways(int new_speed_sideways) { speed_sideways = new_speed_sideways; }
