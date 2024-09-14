#ifndef CAR_OBSTACLE_H
#define CAR_OBSTACLE_H

#include "obstacle.h"

class CarObstacle : public Obstacle {
public:
  // Constructor
  CarObstacle(int start_x, int start_y, int obstacle_speed);

  // Implement the render function specific to CarObstacle
  void render(SDL_Renderer* rend) const override;

  // Implement the update_position function for CarObstacle
  void update_position() override;

  // Static member functions to access private constants
  static int getCarObstacleWidth();
  static int getCarObstacleHeight();
  static int getWheelWidth();
  static int getWheelHeight();

private:
  static const int CAR_OBSTACLE_WIDTH;
  static const int CAR_OBSTACLE_HEIGHT;
  static const int WHEEL_WIDTH;
  static const int WHEEL_HEIGHT;
};

#endif // CAR_OBSTACLE_H
