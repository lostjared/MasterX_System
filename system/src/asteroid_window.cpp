#include "asteroid_window.hpp"
#include <cmath>
#include <cstdlib>
#include <ctime>

namespace mx {

    AsteroidsWindow::AsteroidsWindow(mxApp &app) 
        : mx::Window(app), ship{320, 240, 0, 0}
    {
        screen = SDL_CreateTexture(app.ren, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 640, 480);
        if(!screen) {
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
        if(screen != nullptr) {
            SDL_DestroyTexture(screen);
        }
    }

    void AsteroidsWindow::draw(mxApp &app) {

        if(!Window::isVisible())
            return;

        if(!Window::isDraw()) 
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
        app.printText(25, 25, "Lives: " + std::to_string(lives),{255,255,255,255});
        app.printText(25, 50, "Score: " + std::to_string(score), {255,0,0,255});
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
            if (state[SDL_SCANCODE_SPACE] && currentTime - lastFireTime >= fireCooldown) {
                fireBullet();
                lastFireTime = currentTime;  // Update the last fire time
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
            if (asteroid.x < 0) asteroid.x += 640;
            if (asteroid.x > 640) asteroid.x -= 640;
            if (asteroid.y < 0) asteroid.y += 480;
            if (asteroid.y > 480) asteroid.y -= 480;
        }

        if (asteroids.size() < 5) {
           while (asteroids.size() < 5) {
                spawnAsteroid();  
            }
    }
    }
    void AsteroidsWindow::updateBullets() {
        std::vector<Asteroid> newAsteroids;

        for (auto bulletIt = bullets.begin(); bulletIt != bullets.end(); ) {
            bulletIt->x += bulletIt->dx;
            bulletIt->y += bulletIt->dy;

            if (bulletIt->x < 0 || bulletIt->x > 640 || bulletIt->y < 0 || bulletIt->y > 480) {
                bulletIt = bullets.erase(bulletIt);
                continue;
            }

            bool bulletErased = false;
            for (auto asteroidIt = asteroids.begin(); asteroidIt != asteroids.end(); ) {
                float dist = std::sqrt(std::pow(bulletIt->x - asteroidIt->x, 2) + std::pow(bulletIt->y - asteroidIt->y, 2));

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
        float dist = std::sqrt(std::pow(ship.x - asteroid.x, 2) + std::pow(ship.y - asteroid.y, 2));
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
        if (ship.x < 0) ship.x += 640;
        if (ship.x > 640) ship.x -= 640;
        if (ship.y < 0) ship.y += 480;
        if (ship.y > 480) ship.y -= 480;
        ship.speed *= 0.99;
    }

    template<typename T>
    T minx(const T &a, const T &b) {
        return (a < b) ? a : b;
    }

    template<typename T>
    T maxx(const T &a, const T&b) {
        return (a > b) ? a : b;
    }

    void AsteroidsWindow::drawShip(SDL_Renderer *renderer, int ship_x,int ship_y, float ship_angle) {
        SDL_Color color_inner = {200, 200, 200, 255};  
        SDL_Color color_outer = {100, 100, 100, 255};  

        SDL_Point points[3];
        points[0] = { static_cast<int>(ship_x + 15 * std::cos(ship_angle * M_PI / 180.0)),
                    static_cast<int>(ship_y + 15 * std::sin(ship_angle * M_PI / 180.0)) };
        points[1] = { static_cast<int>(ship_x + 15 * std::cos((ship_angle + 140) * M_PI / 180.0)),
                    static_cast<int>(ship_y + 15 * std::sin((ship_angle + 140) * M_PI / 180.0)) };
        points[2] = { static_cast<int>(ship_x + 15 * std::cos((ship_angle + 220) * M_PI / 180.0)),
                    static_cast<int>(ship_y + 15 * std::sin((ship_angle + 220) * M_PI / 180.0)) };

        int minY = minx(points[0].y, minx(points[1].y, points[2].y));
        int maxY = maxx(points[0].y, maxx(points[1].y, points[2].y));

        for (int y = minY; y <= maxY; y++) {
            int startX = 640, endX = 0;
            for (int i = 0; i < 3; i++) {
                int next = (i + 1) % 3;
                if ((points[i].y <= y && points[next].y > y) || (points[next].y <= y && points[i].y > y)) {
                    float t = (float)(y - points[i].y) / (points[next].y - points[i].y);
                    int x = points[i].x + t * (points[next].x - points[i].x);
                    if (x < startX) {
                        startX = x;
                    }
                    if (x > endX) {
                        endX = x;
                    }
                }
            }

            for (int x = startX; x <= endX; x++) {
                float t = (float)(x - startX) / (endX - startX);
                Uint8 r = color_outer.r * (1 - t) + color_inner.r * t;
                Uint8 g = color_outer.g * (1 - t) + color_inner.g * t;
                Uint8 b = color_outer.b * (1 - t) + color_inner.b * t;
                SDL_SetRenderDrawColor(renderer, r, g, b, 255);
                SDL_RenderDrawPoint(renderer, x, y);
            }
        }
    }

    void AsteroidsWindow::drawAsteroids(SDL_Renderer *renderer) {
        for (const auto &asteroid : asteroids) {
            int rounded_x = static_cast<int>(std::round(asteroid.x));
            int rounded_y = static_cast<int>(std::round(asteroid.y));
            int radius = static_cast<int>(std::round(asteroid.radius));

            draw_circle(renderer, rounded_x, rounded_y, radius);
        }
    }

    void AsteroidsWindow::draw_circle(SDL_Renderer *renderer, int center_x, int center_y, int radius) {
        for (int y = -radius; y <= radius; y++) {
            for (int x = -radius; x <= radius; x++) {
                int dist_sq = x * x + y * y;  
                if (dist_sq <= radius * radius) {
                    float dist = std::sqrt(dist_sq);  
                    float shade = 255 * (1 - (dist / radius));  
                    
                    
                    if (shade < 0) shade = 0;
                    if (shade > 255) shade = 255;

                    SDL_SetRenderDrawColor(renderer, (Uint8)shade, (Uint8)shade, (Uint8)shade, 255);
                    SDL_RenderDrawPoint(renderer, center_x + x, center_y + y);
                }
            }
        }
    }

    void AsteroidsWindow::drawBullets(SDL_Renderer *renderer) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); 
        for (const auto &bullet : bullets) {
            SDL_Rect rect = { static_cast<int>(bullet.x - 2),
                            static_cast<int>(bullet.y - 2),
                            4, 4 };
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
}
