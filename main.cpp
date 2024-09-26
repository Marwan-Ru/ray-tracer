// Ray-tracer.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
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
};

struct Direction {
    float x, y, z;

    Direction(const float x, const float y, const float z) : x(x), y(y), z(z) {}

    [[nodiscard]] float dot(const Direction d2) const {
        return this->x * d2.x + this->y * d2.y + this->z * d2.z;
    }

    [[nodiscard]] float lenght()const {
        return sqrt(this->dot(*this));
    }

    [[nodiscard]] float lenght_squared()const {
        return this->dot(*this);
    }

    Direction operator/(const float val) const {
        return Direction{ this->x / val, this->y / val, this->z / val };
    }

    [[nodiscard]] Direction normalize() const {
        const float len = this->lenght();
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
};

struct Rayon {
    Point origin;
    Direction direction;

    Rayon(const Point origin, const Direction direction) : origin(origin), direction(direction) {}

    [[nodiscard]] float getIntersectionDistance(const float t) const {
        return this->direction.lenght() * t;
    }
};

struct Sphere {
    float radius;
    Point center;
    Color albedo;

    Sphere(const float radius, const Point center, const Color albedo) : radius(radius), center(center), albedo(albedo) {}
};

struct Light {
    Point position;
    Color intensity;

    Light(const Point position, const Color intensity) : position(position), intensity(intensity) {}
};

struct Scene {
    vector<Sphere> spheres;
    vector<Light> lights;

    Scene(const vector<Sphere>& spheres, const vector<Light>& lights) : spheres(spheres), lights(lights) {}

    Scene() : spheres(), lights() {}

    void addSphere(const Sphere& sphere) {
        spheres.push_back(sphere);
    }

    void addLight(const Light& light) {
        lights.push_back(light);
    }
};

struct Intersection {
    bool isIntersection;
    float distance;
    Point intersection;
    Sphere sphere;

    Intersection(const bool isIntersection, const float distance, const Point intersection, const Sphere& sphere) : isIntersection(isIntersection), distance(distance), intersection(intersection), sphere(sphere) {}

    Intersection() : isIntersection(false), distance(0), intersection(Point{ 0,0,0 }), sphere(Sphere(0, Point{ 0,0,0 }, Color::white())) {}
};

float sq(const float val) {
    return val * val;
}

Intersection intersect_sphere(Sphere s, Rayon r) {
    Intersection ret = Intersection();
    Direction oc = r.origin - s.center;

    // Note : we can simplify the value of a if ray direction is normalized
    double a = r.direction.lenght_squared();
    double b = 2.0 * oc.dot(r.direction);
    double c = oc.lenght_squared() - sq(s.radius);

    double delta = b * b - 4.0 * a * c;

    // is intersection (we don't care yet about positive or first)
    if (delta >= 0.0) {
        ret.isIntersection = true;

        double sqdelta = sqrt(delta);
        double t1 = (-b - sqdelta) / (2.0 * a);
        double t2 = (-b + sqdelta) / (2.0 * a);

        if (t1 >= 0.0) {
            ret.distance = t1;

        }
        else if (t2 >= 0.0) {
            ret.distance = t2;
        }

        ret.intersection = r.origin + r.direction * ret.distance;

        ret.sphere = s;
    }
    return ret;
}


int main()
{
    int w = 800;
    int h = 600;


}