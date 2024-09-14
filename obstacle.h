#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <SDL2/SDL.h>

class Obstacle {
public:
  // Constructor for setting obstacle's initial position, size, and speed
  Obstacle(int start_x, int start_y, int obstacle_width, int obstacle_height, int obstacle_speed);

  // Virtual destructor
  virtual ~Obstacle() {}

  // Pure virtual function for rendering the obstacle
  virtual void render(SDL_Renderer* rend) const = 0;

  // Pure virtual function for updating the position
  virtual void update_position() = 0;

  // Getters and setters for position, size, and speed
  int get_x() const;
  void set_x(int new_x);

  int get_y() const;
  void set_y(int new_y);

  int get_width() const;
  void set_width(int new_width);

  int get_height() const;
  void set_height(int new_height);

  int get_speed() const;
  void set_speed(int new_speed);

protected:
  int x, y;
  int width, height;
  int speed;
};

#endif // OBSTACLE_H
