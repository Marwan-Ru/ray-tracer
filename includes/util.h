//
// Created by maaitaddi on 07/10/2024.
//



#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include <fstream>
#include <string>
#include <cmath>


using namespace std;

struct Color {
    float red, green, blue;

    Color(const float red, const float green, const float blue) : red(red), green(green), blue(blue) {}

    Color() : red(0), green(0), blue(0) {}

    static Color white() {
        return Color{ 255, 255, 255 };
    }

    static Color black() {
        return Color{ 0, 0, 0 };
    }

    void cap() {
        this->red = min<float>(this->red, 250.0);
        this->green = min<float>(this->green, 250.0);
        this->blue = min<float>(this->blue, 250.0);

        this->red = max<float>(this->red, 0);
        this->green = max<float>(this->green, 0);
        this->blue = max<float>(this->blue, 0);
    }

    Color operator*(const float val) const {
        return Color{ this->red * val, this->green * val, this->blue * val };
    }

    Color operator+(const Color c2) const {
        return Color{ this->red + c2.red, this->green + c2.green, this->blue + c2.blue };
    }
};

static void write_color(std::ofstream * out, Color c) {

    * out << std::to_string(static_cast<int>(c.red)) << " " << std::to_string(static_cast<int>(c.green)) << " " << std::to_string(static_cast<int>(c.blue)) << " ";
}

// --- Both basically vectors but two different concepts ---

struct Direction {
    float x, y, z;

    Direction(const float x, const float y, const float z) : x(x), y(y), z(z) {}

    [[nodiscard]] float dot(const Direction d2) const {
        return this->x * d2.x + this->y * d2.y + this->z * d2.z;
    }

    [[nodiscard]] float length()const {
        return sqrt(this->dot(*this));
    }

    [[nodiscard]] float length_squared()const {
        return this->dot(*this);
    }

    Direction operator/(const float val) const {
        return Direction{ this->x / val, this->y / val, this->z / val };
    }

    [[nodiscard]] Direction normalize() const {
        const float len = this->length();
        return Direction{ this->x / len, this->y / len, this->z / len };
    }

    [[nodiscard]] Direction inverse() const {
        return Direction{ -this->x, -this->y, -this->z };
    }

    Direction operator*(const float val) const {
        return Direction{ this->x * val, this->y * val, this->z * val };
    }

    Direction operator+(const Direction d2) const {
        return Direction{ this->x + d2.x, this->y + d2.y, this->z + d2.z };
    }
};

inline Direction operator/(const float lhs, const Direction & rhs) {
    return Direction{ lhs / rhs.x, lhs / rhs.y, lhs / rhs.z };
}

struct Point {
    float x, y, z;

    Point(const float x, const float y, const float z) : x(x), y(y), z(z) {}

    Point operator+(const Direction d2) const {
        return Point{ this->x + d2.x, this->y + d2.y, this->z + d2.z };
    }

    Point operator-(const Direction d2) const {
        return Point{ this->x - d2.x, this->y - d2.y, this->z - d2.z };
    }

    Direction operator-(const Point p2) const {
        return Direction{ this->x - p2.x, this->y - p2.y, this->z - p2.z };
    }

    Direction operator/(const float val) const {
        return Direction{ this->x / val, this->y / val, this->z / val };
    }

    [[nodiscard]] Point minp(const Point p2) const {
        return {std::min(this->x, p2.x),std::min(this->y, p2.y),std::min(this->z, p2.z)};
    }

    [[nodiscard]] Point maxp(const Point p2) const {
        return {std::max(this->x, p2.x),std::max(this->y, p2.y),std::max(this->z, p2.z)};
    }
};

inline float sq(const float val) {
    return val * val;
}

#endif //UTIL_H
