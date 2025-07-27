//
// Created by fabian on 7/27/25.
//

#pragma once
#include <SDL.h>

struct Body {
    double x, y;
    double vx, vy;
    double fx, fy;
    double mass;

    Body(const double x, const double y, const double mass)
        : x(x), y(y), vx(0), vy(0), fx(0), fy(0), mass(mass) {}

    void resetForce() {
        fx = fy = 0;
    }

    void addForce(const Body& other) {
        constexpr double G = 10.0 * 6.67430e-3;
        const double dx = other.x - x;
        const double dy = other.y - y;
        const double dist = sqrt(dx*dx + dy*dy + 0.5);
        const double F = (G * mass * other.mass) / (dist * dist);
        fx += F * dx / dist;
        fy += F * dy / dist;
    }

    /*void update(double dt) {
        vx += fx / mass * dt;
        vy += fy / mass * dt;
        x += vx * dt;
        y += vy * dt;
    }*/
    void update(const double dt) {
        vx += fx / mass * dt * 0.5;
        vy += fy / mass * dt * 0.5;
        x  += vx * dt;
        y  += vy * dt;
        vx += fx / mass * dt * 0.5;
        vy += fy / mass * dt * 0.5;
    }


    void draw(SDL_Renderer* renderer) const {
        SDL_RenderDrawPoint(renderer, static_cast<int>(x), static_cast<int>(y));
    }
};
