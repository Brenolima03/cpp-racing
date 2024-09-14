#ifndef MAIN_H
#define MAIN_H

constexpr int SCREEN_WIDTH = 640;
constexpr int SCREEN_HEIGHT = 480;
constexpr int STREET_WIDTH = SCREEN_WIDTH / 2 - 40;
constexpr int CURB_THICKNESS = 5;
constexpr int LEFT_CURB = SCREEN_WIDTH / 2 - STREET_WIDTH / 2 - CURB_THICKNESS / 2;
constexpr int RIGHT_CURB = SCREEN_WIDTH / 2 + STREET_WIDTH / 2 - CURB_THICKNESS / 2;

#endif // MAIN_H
