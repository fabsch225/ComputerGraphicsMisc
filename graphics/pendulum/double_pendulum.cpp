//
// Created by fabian on 8/6/25.
//

#include <SDL2/SDL.h>
#include <cmath>
#include <iostream>
#include <vector>

// ########### Pendulum constants ###########
constexpr double g = 9.81;
constexpr double m1 = 1.0, m2 = 1.0;
constexpr double l1 = 1.0, l2 = 1.0;

// ########### Simulation params ###########
constexpr double dt = 0.01;
constexpr int WINDOW_SIZE = 800;
constexpr int WINDOW_MIDDLE = 400;
constexpr double SCALE = 200.0; // pixels per meter

typedef std::vector<double> State;

// ########### Derivatives function for RK4 ###########
State derivatives(const State& y) {
    const double theta1 = y[0];
    const double theta2 = y[1];
    const double omega1 = y[2];
    const double omega2 = y[3];

    const double delta = theta2 - theta1;

    const double den1 = (m1 + m2) * l1 - m2 * l1 * std::cos(delta) * std::cos(delta);
    const double den2 = (l2 / l1) * den1;

    double domega1 = (
        m2 * l1 * omega1 * omega1 * std::sin(delta) * std::cos(delta) +
        m2 * g * std::sin(theta2) * std::cos(delta) +
        m2 * l2 * omega2 * omega2 * std::sin(delta) -
        (m1 + m2) * g * std::sin(theta1)
    ) / den1;

    double domega2 = (
        -m2 * l2 * omega2 * omega2 * std::sin(delta) * std::cos(delta) +
        (m1 + m2) * (
            g * std::sin(theta1) * std::cos(delta) -
            l1 * omega1 * omega1 * std::sin(delta) -
            g * std::sin(theta2)
        )
    ) / den2;

    return { omega1, omega2, domega1, domega2 };
}

// ########### RK4 integrator step ###########
State rk4_step(const State& y, double dt) {
    const State k1 = derivatives(y);
    State y_temp(4);

    for (int i = 0; i < 4; ++i) y_temp[i] = y[i] + 0.5 * dt * k1[i];
    const State k2 = derivatives(y_temp);

    for (int i = 0; i < 4; ++i) y_temp[i] = y[i] + 0.5 * dt * k2[i];
    const State k3 = derivatives(y_temp);

    for (int i = 0; i < 4; ++i) y_temp[i] = y[i] + dt * k3[i];
    const State k4 = derivatives(y_temp);

    State y_next(4);
    for (int i = 0; i < 4; ++i)
        y_next[i] = y[i] + dt / 6.0 * (k1[i] + 2 * k2[i] + 2 * k3[i] + k4[i]);

    return y_next;
}

// ########### Transform to screen coordinates ###########
void to_screen(const double x, const double y, int& sx, int& sy) {
    sx = static_cast<int>(WINDOW_MIDDLE + x * SCALE);
    sy = static_cast<int>(WINDOW_MIDDLE + y * SCALE);
}

// ########### Main Program ###########
int main() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "Hm? " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("double pendulum",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_SIZE, WINDOW_SIZE, SDL_WINDOW_SHOWN);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    State state = { M_PI / 2, M_PI / 2 + 0.01, 0.0, 0.0 };

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                running = false;
        }

        state = rk4_step(state, dt);

        const double x1 = l1 * std::sin(state[0]);
        const double y1 = l1 * std::cos(state[0]);
        const double x2 = x1 + l2 * std::sin(state[1]);
        const double y2 = y1 + l2 * std::cos(state[1]);

        int sx0, sy0, sx1, sy1, sx2, sy2;
        to_screen(0, 0, sx0, sy0);
        to_screen(x1, y1, sx1, sy1);
        to_screen(x2, y2, sx2, sy2);

        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 200, 50, 50, 255);
        SDL_RenderDrawLine(renderer, sx0, sy0, sx1, sy1);
        SDL_RenderDrawLine(renderer, sx1, sy1, sx2, sy2);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_Rect r1 = { sx1 - 5, sy1 - 5, 10, 10 };
        SDL_Rect r2 = { sx2 - 5, sy2 - 5, 10, 10 };
        SDL_RenderFillRect(renderer, &r1);
        SDL_RenderFillRect(renderer, &r2);

        SDL_RenderPresent(renderer);

        SDL_Delay(10);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
