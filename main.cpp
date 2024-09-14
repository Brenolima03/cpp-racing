#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <thread>
#include <chrono>
#include "car.h"
#include "main.h"
#include "obstacle.h"
#include "car_obstacle.h"
#include "barrier_obstacle.h"
#include <unordered_set>
#include <atomic>
#include <mutex>

constexpr int SPACE_BETWEEN_STREET_LINES = 80;
constexpr int STREET_LINE_HEIGHT = 40;
constexpr int SPEED = 10;
constexpr int NUM_OBSTACLES = 3;
constexpr int REMAINING_LIFE = 10;
constexpr int HEALTH_STICK_WIDTH = 10;
constexpr int HEALTH_STICK_HEIGHT = 40;
constexpr int SPACE_BETWEEN_HEALTH_STICKS = 5;

int street_offset = 0;
int health_bar = 100;

std::atomic<bool> stop_thread{false};  // Flag to signal the thread to stop
// Prevents multiple threads from modifying obstacles at the same time
std::mutex obstacle_mutex;

void render_race_track(SDL_Renderer* rend) {
  // Sets the color of the drawing to white
  SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
  // Creates the left and right curb objects
  SDL_Rect left_curb, right_curb;

  left_curb.w = CURB_THICKNESS;
  left_curb.h = SCREEN_HEIGHT;
  left_curb.x = LEFT_CURB;
  left_curb.y = 0;

  right_curb.w = CURB_THICKNESS;
  right_curb.h = SCREEN_HEIGHT;
  right_curb.x = RIGHT_CURB;
  right_curb.y = 0;

  // Render the curbs
  SDL_RenderFillRect(rend, &left_curb);
  SDL_RenderFillRect(rend, &right_curb);

  int current_y = street_offset;

  while (current_y < SCREEN_HEIGHT) {
    // Creates the street line object and its dimensions and sets its positions
    SDL_Rect street_line;
    street_line.w = 5;
    street_line.h = STREET_LINE_HEIGHT;
    street_line.x = (SCREEN_WIDTH / 2) - (street_line.w / 2);
    street_line.y = current_y;
    // Renders the street lines
    SDL_RenderFillRect(rend, &street_line);
    // Creates the effect of movement
    current_y += STREET_LINE_HEIGHT + SPACE_BETWEEN_STREET_LINES;
  }
}

std::pair<int, int> generate_random_coords() {
  int obstacle_x_coord;
  int obstacle_y_coord;

  // Generates random numbers for coordinates between the 2 curbs
  obstacle_x_coord = rand() % (
    RIGHT_CURB - LEFT_CURB - CURB_THICKNESS - 
    BarrierObstacle::getBarrierObstacleWidth() - 
    CarObstacle::getWheelWidth()
  ) + LEFT_CURB + CURB_THICKNESS;

  // Generates random coordinates starting above see visible screen
  obstacle_y_coord = -(rand() % 
    CarObstacle::getCarObstacleHeight() + 
    CarObstacle::getCarObstacleHeight()
  );

  // Return the pair of coordinates
  return std::make_pair(obstacle_x_coord, obstacle_y_coord);
}

// Checks if the coordinates of an obstacle is too close to the previous one
bool is_too_close(int x1, int y1, int x2, int y2, int min_distance) {
  int dx = x1 - x2;
  int dy = y1 - y2;
  return (dx * dx + dy * dy) < (min_distance * min_distance);
}

std::pair<int, int> generate_coordinates(
  const std::vector<Obstacle*>& obstacles, int min_distance
) {
  // Creates an array for the coordinates for new obstacles
  std::pair<int, int> new_coords;
  bool valid_coords = false;

  while (!valid_coords) {
    new_coords = generate_random_coords();

    for (const auto& obstacle : obstacles) {
      // Gets the coordinates for existing obstacles
      int x = obstacle->get_x();
      int y = obstacle->get_y();

      // Checks if the new coordinates and the existing ones are too close
      if (
        is_too_close(new_coords.first, new_coords.second, x, y, min_distance)
      ) {
        valid_coords = false;
        break;
      } else valid_coords = true;
    }
  }
  return new_coords;
}

void create_obstacles(
  std::vector<Obstacle*>& obstacles, int num_obstacles, int SPEED
) {
  for (int i = 0; i < num_obstacles; ++i) {
    if (stop_thread.load()) return; // Exits if the stop flag is set

    // Creates a pair of random numbers
    std::pair<int, int> coords = generate_random_coords();
    // If the loop counter is an even number, instantiates a car, else a barrier
    if (i % 2 == 0) {
      obstacles.push_back(
        new CarObstacle(coords.first, coords.second, SPEED - 6)
      );
    } else {
      obstacles.push_back(
        new BarrierObstacle(coords.first, coords.second, SPEED)
      );
    }

    // Checks if stop_thread is set before sleeping
    auto start_time = std::chrono::steady_clock::now();
    while (
      std::chrono::steady_clock::now() - start_time < std::chrono::seconds(1)
    ) {
      if (stop_thread.load()) return; // Exits if the stop flag is set

      // Sleeps briefly to allow for flag checking
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
  }
}

void inflict_damage_when_hit(Car& car, std::vector<Obstacle*>& obstacles) {
  // Stores obstacles that have already caused damage
  static std::unordered_set<Obstacle*> active_obstacles;

  int car_x = car.get_x(); // Gets car's x position
  int car_y = car.get_y(); // Gets car's y position
  int car_width = Car::CAR_WIDTH; // Gets car's width
  int car_height = Car::CAR_HEIGHT; // Gets car's height

  bool collision_detected = false; // Tracks if a collision has occurred

  // Checks collision with each obstacle
  for (auto& obstacle : obstacles) {
    int obstacle_x = obstacle->get_x(); // Gets obstacle's x position
    int obstacle_y = obstacle->get_y(); // Gets obstacle's y position
    int obstacle_width = obstacle->get_width(); // Gets obstacle's width
    int obstacle_height = obstacle->get_height(); // Gets obstacle's height

    // Checks if the car and obstacle are overlapping on the x-axis
    bool collision_x = car_x <
      (obstacle_x + obstacle_width) && (car_x + car_width) > obstacle_x;
    // Checks if the car and obstacle are overlapping on the y-axis
    bool collision_y = car_y <
      (obstacle_y + obstacle_height) && (car_y + car_height) > obstacle_y;

    // If both x and y collisions are true, there's a collision
    if (collision_x && collision_y) {
      // If this obstacle hasn't caused damage yet
      if (active_obstacles.find(obstacle) == active_obstacles.end()) {
        health_bar -= 10; // Decrease health bar by 10
        // Marks this obstacle as having caused damage
        active_obstacles.insert(obstacle);
      }
      collision_detected = true; // Collision has occurred
    } else {
      // Removes obstacle if no collision occurs
      active_obstacles.erase(obstacle);
    }
  }

  // If no collision detected, clears the active obstacles set
  if (!collision_detected) {
    active_obstacles.clear();
  }
}

void render_health_bar(SDL_Renderer* rend) {
  int health_sticks = health_bar / 10;

  // Sets the color to white
  SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);

  // Sets the positions and dimentions for the health sticks and renders them
  for (int i = 0; i < health_sticks; ++i) {
    SDL_Rect stick;
    stick.w = HEALTH_STICK_WIDTH;
    stick.h = HEALTH_STICK_HEIGHT;
    stick.x = SCREEN_WIDTH - (HEALTH_STICK_WIDTH + SPACE_BETWEEN_HEALTH_STICKS)
      * (REMAINING_LIFE - i) - 10;
    stick.y = SCREEN_HEIGHT - HEALTH_STICK_HEIGHT - 10;
    SDL_RenderFillRect(rend, &stick);
  }
}

void end_game(
  SDL_Renderer* rend, SDL_Window* window, std::vector<Obstacle*>& obstacles
) {
  // Initializes SDL_ttf library
  if (TTF_Init() == -1) {
    std::cerr << "Failed to initialize SDL_ttf: "
    << TTF_GetError() << std::endl;
    return;
  }

  // Opens the font with the specified size
  TTF_Font* font = TTF_OpenFont("bit5x3.ttf", 48);
  if (!font) {
    std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
    TTF_Quit();
    return;
  }

  SDL_Color textColor = {255, 255, 255, 255}; // Sets the text color to white

  // Creates "You Lose!" text surface
  SDL_Surface* textSurface = TTF_RenderText_Solid(font, "You Lose!", textColor);
  if (!textSurface) {
    std::cerr << "Failed to create text surface: "
    << TTF_GetError() << std::endl;

    TTF_CloseFont(font);
    TTF_Quit();
    return;
  }

  // Creates a texture from the "You Lose!" text surface
  SDL_Texture* textTexture = SDL_CreateTextureFromSurface(rend, textSurface);
  if (!textTexture) {
    std::cerr << "Failed to create text texture: "
    << SDL_GetError() << std::endl;

    SDL_FreeSurface(textSurface);
    TTF_CloseFont(font);
    TTF_Quit();
    return;
  }

  // Creates "Press Enter to play again" text surface
  SDL_Surface* promptSurface = TTF_RenderText_Solid(font, "Press Enter to play again", textColor);

  if (!promptSurface) {
    std::cerr << "Failed to create prompt surface: "
    << TTF_GetError() << std::endl;

    SDL_DestroyTexture(textTexture);
    SDL_FreeSurface(textSurface);
    TTF_CloseFont(font);
    TTF_Quit();
    return;
  }

  // Creates a texture from the "Press Enter to play again" text surface
  SDL_Texture* promptTexture = SDL_CreateTextureFromSurface(rend, promptSurface);

  if (!promptTexture) {
    std::cerr << "Failed to create prompt texture: "
    << SDL_GetError() << std::endl;

    SDL_FreeSurface(promptSurface);
    SDL_DestroyTexture(textTexture);
    SDL_FreeSurface(textSurface);
    TTF_CloseFont(font);
    TTF_Quit();
    return;
  }

  SDL_FreeSurface(textSurface);
  SDL_FreeSurface(promptSurface);

  // Set positions for the texts
  SDL_Rect textRect;
  textRect.x = (SCREEN_WIDTH - textSurface->w) / 2;
  textRect.y = SCREEN_HEIGHT / 2 - textSurface->h / 2 - 30;
  textRect.w = textSurface->w;
  textRect.h = textSurface->h;

  SDL_Rect promptRect;
  promptRect.x = (SCREEN_WIDTH - promptSurface->w) / 2;
  promptRect.y = textRect.y + textRect.h + 10;
  promptRect.w = promptSurface->w;
  promptRect.h = promptSurface->h;

  SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
  SDL_RenderClear(rend);
  SDL_RenderCopy(rend, textTexture, nullptr, &textRect);
  SDL_RenderCopy(rend, promptTexture, nullptr, &promptRect);
  SDL_RenderPresent(rend);

  SDL_DestroyTexture(textTexture);
  SDL_DestroyTexture(promptTexture);
  TTF_CloseFont(font);
  TTF_Quit();

  for (auto& obstacle : obstacles) {
    delete obstacle;
  }
  obstacles.clear();
}

void play_again(
  SDL_Renderer* rend, SDL_Window* window,
  std::vector<Obstacle*>& obstacles,
  Car& car, std::thread& obstacle_thread,
  std::atomic<bool>& stop_thread, bool& game_over
) {
  stop_thread.store(true);
  if (obstacle_thread.joinable()) {
    obstacle_thread.join();
  }

  for (auto& obstacle : obstacles) delete obstacle;

  obstacles.clear();

  car.set_x(SCREEN_WIDTH / 2 - Car::CAR_WIDTH / 2);
  car.set_y(SCREEN_HEIGHT / 2 + 100);
  health_bar = 100;
  street_offset = 0;
  game_over = false;
  stop_thread.store(false);

  obstacle_thread = std::thread(
    create_obstacles, std::ref(obstacles), NUM_OBSTACLES, SPEED
  );
}

int main(int argc, char* argv[]) {
  // Creates a seed to generate random numbers
  std::srand(std::time(nullptr));

  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    std::cout << "Error initializing SDL: " << SDL_GetError() << std::endl;
    return 1;
  }

  SDL_Window* window = SDL_CreateWindow(
    "Racing",
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    SCREEN_WIDTH,
    SCREEN_HEIGHT,
    0
  );

  if (!window) {
    std::cout << "Error creating window: " << SDL_GetError() << std::endl;
    SDL_Quit();
    return 1;
  }

  SDL_Renderer* rend = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!rend) {
    std::cout << "Error creating renderer: " << SDL_GetError() << std::endl;
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  // Instantiates the car
  Car car(SCREEN_WIDTH / 2 - Car::CAR_WIDTH / 2, SCREEN_HEIGHT / 2 + 100, 5);

  // Creates an array for the obstacle coordinates
  std::vector<Obstacle*> obstacles;
  // Starts a new thread to create obstacles with specified number and speed
  std::thread obstacle_thread(
    create_obstacles, std::ref(obstacles), NUM_OBSTACLES, SPEED
  );


  bool close = false;
  bool game_over = false;

  while (!close) {
    SDL_Event event;
    const Uint8* keystates = SDL_GetKeyboardState(NULL);

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) close = true;

      if (keystates[SDL_SCANCODE_ESCAPE]) {
        close = true;
        stop_thread.store(true);
        if (obstacle_thread.joinable()) {
          obstacle_thread.join();
        }
      }

      if (health_bar == 0 && !game_over) {
        game_over = true;
        stop_thread.store(true);

        if (obstacle_thread.joinable()) {
          obstacle_thread.join();
        }

        end_game(rend, window, obstacles);
        continue;
      }

      if (game_over)
        if (keystates[SDL_SCANCODE_RETURN]) {
          play_again(
            rend, window, obstacles, car,
            obstacle_thread, stop_thread, game_over
          );
          continue;
        }
    }

    if (!game_over) {
      // Allows the car to move sideways using the keyboard
      car.move_sideways(keystates);

      // Creates the effect of movement by making the
      // offset receiving the value of the speed
      street_offset += SPEED;
      if (street_offset >= -(STREET_LINE_HEIGHT - SPACE_BETWEEN_STREET_LINES)
      ) street_offset -= STREET_LINE_HEIGHT + SPACE_BETWEEN_STREET_LINES;

      // Updates the position of each obstacle
      for (auto& obstacle : obstacles) {
        obstacle->update_position();  // Moves the obstacle
        
        // If the obstacle moves off the screen, repositions it
        if (obstacle->get_y() > SCREEN_HEIGHT) {
          std::pair<int, int> new_coords = generate_coordinates(obstacles, 100);
          obstacle->set_x(new_coords.first);  // Sets new x-coordinate
          obstacle->set_y(new_coords.second); // Sets new y-coordinate
        }
      }
      inflict_damage_when_hit(car, obstacles);

      SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
      SDL_RenderClear(rend);

      render_race_track(rend);
      car.render(rend);

      for (const auto& obstacle : obstacles) {
        obstacle->render(rend);
      }
      render_health_bar(rend);
      SDL_RenderPresent(rend);
      SDL_Delay(1000 / 60);
    }
  }

  // Ensures the obstacle thread is joined if it is joinable
  if (obstacle_thread.joinable()) {
    stop_thread.store(true);  // Signals the thread to stop
    obstacle_thread.join();   // Waits for the thread to finish
  }


  // Frees the memory
  for (auto& obstacle : obstacles) {
    delete obstacle;
  }
  
  SDL_DestroyRenderer(rend);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
