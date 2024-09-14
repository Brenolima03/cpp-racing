#include "barrier_obstacle.h"

BarrierObstacle::BarrierObstacle(int start_x, int start_y, int obstacle_speed)
  : Obstacle(
    start_x, start_y, BARRIER_OBSTACLE_WIDTH,BARRIER_OBSTACLE_HEIGHT, obstacle_speed) {}

void BarrierObstacle::render(SDL_Renderer* rend) const {
  SDL_Rect barrier_body;
  barrier_body.x = get_x();
  barrier_body.y = get_y();
  barrier_body.w = get_width();
  barrier_body.h = get_height();

  SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
  SDL_RenderFillRect(rend, &barrier_body);
}

void BarrierObstacle::update_position() {
  set_y(get_y() + get_speed());
}

int BarrierObstacle::getBarrierObstacleWidth() {
  return BARRIER_OBSTACLE_WIDTH;
}

int BarrierObstacle::getBarrierObstacleHeight() {
  return BARRIER_OBSTACLE_HEIGHT;
}
