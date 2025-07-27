//
// Created by fabian on 7/27/25.
//

#include <SDL.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <random>
#include "body.h"
#include "quad.h"
#include "barnes_hut_tree.h"

constexpr double G = 10.0 * 6.67430e-3;
constexpr int WIDTH = 800, HEIGHT = 800;
constexpr int NUM_BODIES_PER_SPIRAL = 4000;
constexpr double DT = 0.4;
constexpr double BLACK_HOLE_MASS = 2000;

void createSpiral(std::vector<Body>& bodies, double cx, double cy, int direction) {
    for (int i = 0; i < NUM_BODIES_PER_SPIRAL; ++i) {
        const double angle = i * 0.05;
        const double radius = 10 + i * 0.025;
        const double x = cx + radius * cos(angle) + (rand() % 20) / 20.0;
        const double y = cy + radius * sin(angle) + (rand() % 20) / 20.0;;
        constexpr double mass = 0.1;


        double dx = x - cx;
        double dy = y - cy;
        double dist = sqrt(dx*dx + dy*dy);
        double v = sqrt(G * BLACK_HOLE_MASS / dist);

        double vx = direction * -v * dy / dist;
        double vy = direction * v * dx / dist;

        Body b(x, y, mass);
        b.vx = vx;
        b.vy = vy;
        bodies.push_back(b);
    }
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Barnes-Hut Gravity",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    std::srand(static_cast<unsigned>(std::time(0)));
    std::vector<Body> bodies;

    const Body blackHole1(WIDTH / 2.0 + 200, HEIGHT / 2.0, BLACK_HOLE_MASS);
    const Body blackHole2(WIDTH / 2.0 - 200, HEIGHT / 2.0, BLACK_HOLE_MASS);
    bodies.push_back(blackHole1);
    bodies.push_back(blackHole2);

    createSpiral(bodies, blackHole1.x, blackHole1.y, +1);
    createSpiral(bodies, blackHole2.x, blackHole2.y, -1);

    bool running = true;
    SDL_Event e;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
        }

        Quad rootQuad(WIDTH / 2, HEIGHT / 2, WIDTH);
        BHTree tree(rootQuad);

        for (auto& body : bodies)
            if (rootQuad.contains(body.x, body.y))
                tree.insert(&body);

        for (auto& body : bodies) {
            body.resetForce();
            tree.updateForce(&body);
        }

        for (auto& body : bodies)
            body.update(DT);


        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        for (size_t i = 0; i < bodies.size(); ++i) {
            if (i == 0 || i == 1)
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            else
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

            bodies[i].draw(renderer);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(8);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
