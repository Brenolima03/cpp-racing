#ifndef BARRIER_OBSTACLE_H
#define BARRIER_OBSTACLE_H

#include "obstacle.h"

class BarrierObstacle : public Obstacle {
public:
  // Constructor
  BarrierObstacle(int start_x, int start_y, int obstacle_speed);

  // Implement the render function specific to BarrierObstacle
  void render(SDL_Renderer* rend) const override;

  // Implement the update_position function for BarrierObstacle
  void update_position() override;

  // Static methods to access private constants
  static int getBarrierObstacleWidth();
  static int getBarrierObstacleHeight();

private:
  static const int BARRIER_OBSTACLE_WIDTH = 60;
  static const int BARRIER_OBSTACLE_HEIGHT = 10;
};

#endif // BARRIER_OBSTACLE_H
