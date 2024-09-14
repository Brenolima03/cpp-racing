#include "obstacle.h"

Obstacle::Obstacle(
  int start_x, int start_y, int obstacle_width,
  int obstacle_height, int obstacle_speed
)
  : x(start_x), y(start_y), width(obstacle_width),
  height(obstacle_height), speed(obstacle_speed) {}

int Obstacle::get_x() const { return x; }
void Obstacle::set_x(int new_x) { x = new_x; }

int Obstacle::get_y() const { return y; }
void Obstacle::set_y(int new_y) { y = new_y; }

int Obstacle::get_width() const { return width; }
void Obstacle::set_width(int new_width) { width = new_width; }

int Obstacle::get_height() const { return height; }
void Obstacle::set_height(int new_height) { height = new_height; }

int Obstacle::get_speed() const { return speed; }
void Obstacle::set_speed(int new_speed) { speed = new_speed; }
