#include "car_obstacle.h"

// Define static constants
const int CarObstacle::CAR_OBSTACLE_WIDTH = 30;
const int CarObstacle::CAR_OBSTACLE_HEIGHT = 70;
const int CarObstacle::WHEEL_WIDTH = CAR_OBSTACLE_WIDTH / 8;
const int CarObstacle::WHEEL_HEIGHT = CAR_OBSTACLE_HEIGHT / 5;

CarObstacle::CarObstacle(int start_x, int start_y, int obstacle_speed)
  : Obstacle(
    start_x, start_y, CAR_OBSTACLE_WIDTH, CAR_OBSTACLE_HEIGHT, obstacle_speed
  ) {}

void CarObstacle::render(SDL_Renderer* rend) const {
  SDL_Rect car_body;
  car_body.x = get_x();
  car_body.y = get_y();
  car_body.w = get_width();
  car_body.h = get_height();

  SDL_SetRenderDrawColor(rend, 255, 0, 0, 255);
  SDL_RenderFillRect(rend, &car_body);

  SDL_SetRenderDrawColor(rend, 255, 0, 0, 255);
  SDL_Rect wheel;

  wheel.w = getWheelWidth();
  wheel.h = getWheelHeight();

  // Render the four wheels
  wheel.x = get_x() - getWheelWidth();
  wheel.y = get_y() + get_height() / 6;
  SDL_RenderFillRect(rend, &wheel);

  wheel.x = get_x() + get_width();
  SDL_RenderFillRect(rend, &wheel);

  wheel.x = get_x() - getWheelWidth();
  wheel.y = get_y() + get_height() - getWheelHeight() - 5;
  SDL_RenderFillRect(rend, &wheel);

  wheel.x = get_x() + get_width();
  SDL_RenderFillRect(rend, &wheel);
}

void CarObstacle::update_position() {
  set_y(get_y() + get_speed());
}

// Define the getter methods
int CarObstacle::getCarObstacleWidth() {
  return CAR_OBSTACLE_WIDTH;
}

int CarObstacle::getCarObstacleHeight() {
  return CAR_OBSTACLE_HEIGHT;
}

int CarObstacle::getWheelWidth() {
  return WHEEL_WIDTH;
}

int CarObstacle::getWheelHeight() {
  return WHEEL_HEIGHT;
}
