#ifndef __ASTEROIDS__H_
#define __ASTEROIDS__H__

#include"mx_window.hpp"

namespace mx {
    class AsteroidsWindow : public Window {
    public:
        AsteroidsWindow(mxApp &app);
        virtual ~AsteroidsWindow();
        
        virtual void draw(mxApp &app) override;
        virtual bool event(mxApp &app, SDL_Event &e) override;
        void newGame();
    private:
        struct Asteroid {
            float x, y;
            float dx, dy;
            float radius;
        };
        
        struct Bullet {
            float x, y;
            float dx, dy;
        };
        
        struct Ship {
            float x, y;
            float angle;
            float speed;
        };
        
        Ship ship;
        std::vector<Asteroid> asteroids;
        std::vector<Bullet> bullets;

        int lives = 3;
        int score = 0;
        
        void resetShip();
        void checkShipCollision();
        void spawnAsteroid();
        void updateAsteroids();
        void updateBullets();
        void updateShip();
        void drawShip(SDL_Renderer *renderer, int x, int y, float angle);
        void drawAsteroids(SDL_Renderer *renderer);
        void drawBullets(SDL_Renderer *renderer);
        void fireBullet();
        void draw_circle(SDL_Renderer *renderer, int center_x, int center_y, int radius);
        SDL_Texture *screen = nullptr;
    };
}

#endif