#include "asteroid_window.hpp"
#include <cmath>
#include <cstdlib>
#include <ctime>

namespace mx {

AsteroidsWindow::AsteroidsWindow(mxApp &app)
    : mx::Window(app), ship{320, 240, 0, 0} {
  screen = SDL_CreateTexture(app.ren, SDL_PIXELFORMAT_RGBA8888,
                             SDL_TEXTUREACCESS_TARGET, 640, 480);
  if (!screen) {
    mx::system_err << "MasterX System: Error creating Asteroid texture..\n";
    mx::system_err.flush();
    exit(EXIT_FAILURE);
  }

  std::srand(static_cast<unsigned>(std::time(0)));

  for (int i = 0; i < 5; ++i) {
    spawnAsteroid();
  }
}

AsteroidsWindow::~AsteroidsWindow() {
  if (screen != nullptr) {
    SDL_DestroyTexture(screen);
  }
}

void AsteroidsWindow::screenResize(int w, int h) {
  const int baseWidth = 1280;
  const int baseHeight = 720;
  int screenWidth = w;
  int screenHeight = h;
  float scaleX = static_cast<float>(screenWidth) / baseWidth;
  float scaleY = static_cast<float>(screenHeight) / baseHeight;
  int windowWidth = static_cast<int>(640 * scaleX);
  int windowHeight = static_cast<int>(480 * scaleY);
  int windowPosX = (screenWidth - windowWidth) / 2;
  int windowPosY = (screenHeight - windowHeight) / 2;
  SDL_Rect rc = {windowPosX, windowPosY, windowWidth, windowHeight};
  setRect(rc);
}

void AsteroidsWindow::draw(mxApp &app) {

  if (!Window::isVisible())
    return;

  if (!Window::isDraw())
    return;

  Window::draw(app);
  Window::drawMenubar(app);

  SDL_Renderer *renderer = app.ren;

  SDL_SetRenderTarget(renderer, screen);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  drawShip(renderer, ship.x, ship.y, ship.angle);
  drawAsteroids(renderer);
  drawBullets(renderer);
  app.printText(25, 25, "Lives: " + std::to_string(lives),
                {255, 255, 255, 255});
  app.printText(25, 50, "Score: " + std::to_string(score), {255, 0, 0, 255});
  SDL_SetRenderTarget(renderer, app.tex);
  SDL_Rect rc;
  Window::getDrawRect(rc);
  SDL_RenderCopy(renderer, screen, nullptr, &rc);
  static Uint32 lastUpdate = SDL_GetTicks();
  Uint32 current = SDL_GetTicks();
  Uint32 delta = current - lastUpdate;

  if (delta >= 16) {
    lastUpdate = current;
    updateShip();
    updateAsteroids();
    updateBullets();
    checkShipCollision();
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    if (state[SDL_SCANCODE_LEFT]) {
      ship.angle -= 5;
    }
    if (state[SDL_SCANCODE_RIGHT]) {
      ship.angle += 5;
    }
    if (state[SDL_SCANCODE_UP]) {
      ship.speed += 0.1;
    }

    static Uint32 lastFireTime = 0;
    static const Uint32 fireCooldown = 300;
    Uint32 currentTime = SDL_GetTicks();
    if (state[SDL_SCANCODE_SPACE] &&
        currentTime - lastFireTime >= fireCooldown) {
      fireBullet();
      lastFireTime = currentTime; // Update the last fire time
    }
  }
}

void AsteroidsWindow::newGame() {
  score = 0;
  lives = 3;
  resetShip();
}

bool AsteroidsWindow::event(mxApp &app, SDL_Event &e) {

  return Window::event(app, e);
}

void AsteroidsWindow::spawnAsteroid() {
  Asteroid asteroid;
  asteroid.x = static_cast<float>(std::rand() % 640);
  asteroid.y = static_cast<float>(std::rand() % 480);
  asteroid.dx = (std::rand() % 100 - 50) / 100.0f;
  asteroid.dy = (std::rand() % 100 - 50) / 100.0f;
  asteroid.radius = 20 + std::rand() % 30;
  asteroids.push_back(asteroid);
}

void AsteroidsWindow::updateAsteroids() {
  for (auto &asteroid : asteroids) {
    asteroid.x += asteroid.dx;
    asteroid.y += asteroid.dy;
    if (asteroid.x < 0)
      asteroid.x += 640;
    if (asteroid.x > 640)
      asteroid.x -= 640;
    if (asteroid.y < 0)
      asteroid.y += 480;
    if (asteroid.y > 480)
      asteroid.y -= 480;
  }

  if (asteroids.size() < 5) {
    while (asteroids.size() < 5) {
      spawnAsteroid();
    }
  }
}
void AsteroidsWindow::updateBullets() {
  std::vector<Asteroid> newAsteroids;

  for (auto bulletIt = bullets.begin(); bulletIt != bullets.end();) {
    bulletIt->x += bulletIt->dx;
    bulletIt->y += bulletIt->dy;

    if (bulletIt->x < 0 || bulletIt->x > 640 || bulletIt->y < 0 ||
        bulletIt->y > 480) {
      bulletIt = bullets.erase(bulletIt);
      continue;
    }

    bool bulletErased = false;
    for (auto asteroidIt = asteroids.begin(); asteroidIt != asteroids.end();) {
      float dist = std::sqrt(std::pow(bulletIt->x - asteroidIt->x, 2) +
                             std::pow(bulletIt->y - asteroidIt->y, 2));

      if (dist <= asteroidIt->radius) {
        bulletIt = bullets.erase(bulletIt);
        bulletErased = true;
        score += 100;
        if (asteroidIt->radius > 10) {
          for (int i = 0; i < 2; ++i) {
            Asteroid smallAsteroid;
            smallAsteroid.x = asteroidIt->x;
            smallAsteroid.y = asteroidIt->y;
            smallAsteroid.radius = asteroidIt->radius / 2;
            smallAsteroid.dx = (std::rand() % 100 - 50) / 100.0f;
            smallAsteroid.dy = (std::rand() % 100 - 50) / 100.0f;
            newAsteroids.push_back(smallAsteroid);
          }
        }

        asteroidIt = asteroids.erase(asteroidIt);
        break;
      } else {
        ++asteroidIt;
      }
    }

    if (!bulletErased) {
      ++bulletIt;
    }
  }

  asteroids.insert(asteroids.end(), newAsteroids.begin(), newAsteroids.end());
}

void AsteroidsWindow::resetShip() {
  ship.x = 320;
  ship.y = 240;
  ship.angle = 0;
  ship.speed = 0;
}

void AsteroidsWindow::checkShipCollision() {
  for (const auto &asteroid : asteroids) {
    float dist = std::sqrt(std::pow(ship.x - asteroid.x, 2) +
                           std::pow(ship.y - asteroid.y, 2));
    if (dist <= asteroid.radius) {
      lives--;
      if (lives > 0) {
        resetShip();
      } else {
        lives = 3;
        score = 0;
        resetShip();
      }
      break;
    }
  }
}

void AsteroidsWindow::updateShip() {
  ship.x += ship.speed * std::cos(ship.angle * M_PI / 180.0);
  ship.y += ship.speed * std::sin(ship.angle * M_PI / 180.0);
  if (ship.x < 0)
    ship.x += 640;
  if (ship.x > 640)
    ship.x -= 640;
  if (ship.y < 0)
    ship.y += 480;
  if (ship.y > 480)
    ship.y -= 480;
  ship.speed *= 0.99;
}

void AsteroidsWindow::drawShip(SDL_Renderer *renderer, int ship_x, int ship_y,
                               float ship_angle) {
  // Pseudo-3D wireframe fighter — all SDL_RenderDrawLine, no pixel loops.
  float ang = ship_angle * M_PI / 180.f;
  float cosA = std::cos(ang), sinA = std::sin(ang);

  // Rotate a ship-local (lx, ly) point to screen space
  auto proj = [&](float lx, float ly) -> SDL_Point {
    return {ship_x + static_cast<int>(lx * cosA - ly * sinA),
            ship_y + static_cast<int>(lx * sinA + ly * cosA)};
  };

  SDL_Point nose = proj(16.f, 0.f);    // nose tip
  SDL_Point lWing = proj(-9.f, -12.f); // left wing tip
  SDL_Point rWing = proj(-9.f, 12.f);  // right wing tip
  SDL_Point lTail = proj(-14.f, -5.f); // left tail corner
  SDL_Point rTail = proj(-14.f, 5.f);  // right tail corner
  SDL_Point cMid = proj(-9.f, 0.f);    // rear center (engine)
  SDL_Point cFwd = proj(0.f, 0.f);     // center fuselage mid

  // Leading edges nose→wings — bright cyan
  SDL_SetRenderDrawColor(renderer, 0, 240, 200, 255);
  SDL_RenderDrawLine(renderer, nose.x, nose.y, lWing.x, lWing.y);
  SDL_RenderDrawLine(renderer, nose.x, nose.y, rWing.x, rWing.y);

  // Wing trailing edges wings→tail — medium blue
  SDL_SetRenderDrawColor(renderer, 0, 160, 230, 255);
  SDL_RenderDrawLine(renderer, lWing.x, lWing.y, lTail.x, lTail.y);
  SDL_RenderDrawLine(renderer, rWing.x, rWing.y, rTail.x, rTail.y);

  // Tail cross piece — dim blue
  SDL_SetRenderDrawColor(renderer, 0, 100, 180, 255);
  SDL_RenderDrawLine(renderer, lTail.x, lTail.y, rTail.x, rTail.y);

  // Center spine nose→rear — near-white
  SDL_SetRenderDrawColor(renderer, 200, 240, 255, 255);
  SDL_RenderDrawLine(renderer, nose.x, nose.y, cMid.x, cMid.y);

  // Fuselage cross-ribs — dim interior detail
  SDL_SetRenderDrawColor(renderer, 0, 120, 180, 255);
  SDL_RenderDrawLine(renderer, lWing.x, lWing.y, cFwd.x, cFwd.y);
  SDL_RenderDrawLine(renderer, rWing.x, rWing.y, cFwd.x, cFwd.y);

  // Engine glow — orange rect at rear center
  SDL_SetRenderDrawColor(renderer, 255, 130, 0, 255);
  SDL_Rect eng = {cMid.x - 2, cMid.y - 2, 5, 5};
  SDL_RenderFillRect(renderer, &eng);
}

void AsteroidsWindow::drawAsteroids(SDL_Renderer *renderer) {
  for (const auto &asteroid : asteroids) {
    int rounded_x = static_cast<int>(std::round(asteroid.x));
    int rounded_y = static_cast<int>(std::round(asteroid.y));
    int radius = static_cast<int>(std::round(asteroid.radius));

    draw_circle(renderer, rounded_x, rounded_y, radius);
  }
}

void AsteroidsWindow::draw_circle(SDL_Renderer *renderer, int center_x,
                                  int center_y, int radius) {
  // Pseudo-3D wireframe sphere: draw latitude ellipses + a longitude meridian
  // using SDL_RenderDrawLine — O(segs) calls instead of O(r²) point calls.
  const int segs = 32;
  float r = static_cast<float>(radius);
  int cx = center_x, cy = center_y;

  auto drawEllipse = [&](float offY, float rx, float ry, Uint8 cr, Uint8 cg,
                         Uint8 cb) {
    SDL_SetRenderDrawColor(renderer, cr, cg, cb, 255);
    float x0 = cx + rx, y0 = cy + offY;
    for (int i = 1; i <= segs; ++i) {
      float a = 2.f * M_PI * i / segs;
      float x1 = cx + rx * std::cos(a);
      float y1 = cy + offY + ry * std::sin(a);
      SDL_RenderDrawLine(renderer, static_cast<int>(x0), static_cast<int>(y0),
                         static_cast<int>(x1), static_cast<int>(y1));
      x0 = x1;
      y0 = y1;
    }
  };

  // Equator — full circle, bright cyan
  drawEllipse(0.f, r, r, 80, 210, 255);
  // 30° north / south latitude — compressed flat ellipses
  drawEllipse(-r * 0.50f, r * 0.866f, r * 0.13f, 50, 160, 220);
  drawEllipse(r * 0.50f, r * 0.866f, r * 0.13f, 50, 160, 220);
  // 60° north / south latitude — more compressed, dimmer
  drawEllipse(-r * 0.87f, r * 0.50f, r * 0.08f, 25, 100, 165);
  drawEllipse(r * 0.87f, r * 0.50f, r * 0.08f, 25, 100, 165);
  // Vertical meridian (longitude line) — dim teal ellipse, compressed
  // horizontally
  SDL_SetRenderDrawColor(renderer, 35, 125, 185, 255);
  float x0m = cx + r * 0.28f, y0m = static_cast<float>(cy);
  for (int i = 1; i <= segs; ++i) {
    float a = 2.f * M_PI * i / segs;
    float x1 = cx + r * 0.28f * std::cos(a);
    float y1 = cy + r * std::sin(a);
    SDL_RenderDrawLine(renderer, static_cast<int>(x0m), static_cast<int>(y0m),
                       static_cast<int>(x1), static_cast<int>(y1));
    x0m = x1;
    y0m = y1;
  }
}

void AsteroidsWindow::drawBullets(SDL_Renderer *renderer) {
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  for (const auto &bullet : bullets) {
    SDL_Rect rect = {static_cast<int>(bullet.x - 2),
                     static_cast<int>(bullet.y - 2), 4, 4};
    SDL_RenderFillRect(renderer, &rect);
  }
}

void AsteroidsWindow::fireBullet() {
  Bullet bullet;
  bullet.x = ship.x;
  bullet.y = ship.y;
  bullet.dx = 5 * std::cos(ship.angle * M_PI / 180.0);
  bullet.dy = 5 * std::sin(ship.angle * M_PI / 180.0);
  bullets.push_back(bullet);
}
} // namespace mx
