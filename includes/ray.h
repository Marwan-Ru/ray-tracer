//
// Created by maaitaddi on 07/10/2024.
//

#ifndef RAY_H
#define RAY_H

#include "util.h"

struct Ray {
    Point origin;
    Direction direction;
    Direction inv_direction;

    Ray(const Point origin, const Direction direction) : origin(origin), direction(direction), inv_direction(1 / direction) {}

    [[nodiscard]] float getIntersectionDistance(const float t) const {
        return this->direction.length() * t;
    }
};

#endif //RAY_H
