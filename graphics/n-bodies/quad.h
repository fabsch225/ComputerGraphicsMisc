//
// Created by fabian on 7/27/25.
//

#pragma once

struct Quad {
    double x_mid, y_mid, length;

    Quad(const double x_mid, const double y_mid, const double length)
        : x_mid(x_mid), y_mid(y_mid), length(length) {}

    [[nodiscard]] bool contains(const double x, const double y) const {
        return x >= x_mid - length/2 && x <= x_mid + length/2 &&
               y >= y_mid - length/2 && y <= y_mid + length/2;
    }

    [[nodiscard]] Quad NW() const { return { x_mid - length/4, y_mid - length/4, length/2 }; }
    [[nodiscard]] Quad NE() const { return { x_mid + length/4, y_mid - length/4, length/2 }; }
    [[nodiscard]] Quad SW() const { return { x_mid - length/4, y_mid + length/4, length/2 }; }
    [[nodiscard]] Quad SE() const { return { x_mid + length/4, y_mid + length/4, length/2 }; }
};
