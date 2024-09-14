#ifndef CAR_H
#define CAR_H

#include <SDL2/SDL.h>

class Car {
public:
  Car(int start_x, int start_y, int car_side_speed);

  void move_sideways(const Uint8* keystates);
  void render(SDL_Renderer* rend);

  int get_x() const;
  void set_x(int new_x);

  int get_y() const;
  void set_y(int new_y);

  int get_width() const;
  void set_width(int new_width);

  int get_height() const;
  void set_height(int new_height);

  int get_speed_sideways() const;
  void set_speed_sideways(int new_speed_sideways);

  static const int CAR_WIDTH = 30;
  static const int CAR_HEIGHT = 70;
  static const int CAR_HALF_WIDTH = CAR_WIDTH / 2;
  static const int wheel_width = CAR_WIDTH / 8;
  static const int wheel_height = CAR_HEIGHT / 5;
private:
  int x, y, width, height, speed_sideways;
};

#endif // CAR_H
