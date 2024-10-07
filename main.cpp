// Ray-tracer.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//

// ReSharper disable CppUseAuto
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

    Direction operator/(const float val) const {
        return Direction{ this->x / val, this->y / val, this->z / val };
    }
};


Direction operator/(const float lhs, const Direction & rhs) {
    return Direction{ lhs / rhs.x, lhs / rhs.y, lhs / rhs.z };
}

struct Rayon {
    Point origin;
    Direction direction;
    Direction inv_direction;

    Rayon(const Point origin, const Direction direction) : origin(origin), direction(direction), inv_direction(1 / direction) {}

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
    float intensity;

    Light(const Point position, const float intensity) : position(position), intensity(intensity) {}
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
    float t;
    Point intersection;
    Sphere sphere;

    Intersection(const bool isIntersection, const float t, const Point intersection, const Sphere& sphere) : isIntersection(isIntersection), t(t), intersection(intersection), sphere(sphere) {}

    Intersection() : isIntersection(false), t(-INFINITY), intersection(Point{ 0,0,0 }), sphere(Sphere(0, Point{ 0,0,0 }, Color::white())) {}
};

float sq(const float val) {
    return val * val;
}

Intersection intersect_sphere(const Sphere &s, const Rayon &r) {
    Intersection ret = Intersection();
    const Direction oc = r.origin - s.center;

    // Note : we can simplify the value of a if ray direction is normalized
    const double a = r.direction.lenght_squared();
    const double b = 2.0 * oc.dot(r.direction);
    const double c = oc.lenght_squared() - sq(s.radius);

    // is intersection (we don't care yet about positive or first)
    if (const double delta = b * b - 4.0 * a * c; delta >= 0.0) {

        double sqdelta = sqrt(delta);
        const double t1 = (-b - sqdelta) / (2.0 * a);
        const double t2 = (-b + sqdelta) / (2.0 * a);

        if (t1 >= 0.0 && t2 >= 0.0) {
            ret.t = min(t1, t2); // Choose the closer intersection
        } else if (t1 >= 0.0) {
            ret.t = t1;

        }
        else if (t2 >= 0.0) {
            ret.t = t2;
        }

        if(ret.t >= 0.0) {
            ret.isIntersection = true;
            ret.intersection = r.origin + r.direction * ret.t;
            ret.sphere = s;
        }

    }
    return ret;
}

Intersection intersect_spheres(const Scene& s, const Rayon &r) {
    float closest = INFINITY;
    Intersection ret = Intersection();


    for (auto sphere : s.spheres) {
        if(const Intersection int_i = intersect_sphere(sphere, r); int_i.isIntersection && int_i.t < closest && int_i.t >= 0) {
            closest = int_i.t;
            ret = int_i;
        }
    }

    return ret;
}

/*Calculates light visibility for a given light and point*/
float visibility(const Scene& S, const Light l, const Point p) {
    const Direction dir = (l.position - p).normalize();
    const auto r = Rayon(p + dir * 0.1, dir);

    const float light_distance = (l.position - p).lenght();

    if(const Intersection ret = intersect_spheres(S, r); ret.isIntersection) {
        if (const float intersection_distance = (ret.intersection - p).lenght(); intersection_distance < light_distance) {
            return 0;
        }
    }

    return 1;
}

Scene Cornell_box() {
    Scene S = Scene();

    // Box
    S.addSphere( Sphere(1e5, Point{ 1e5+1,40.8,81.6 }, Color::white()) ); //left
    S.addSphere( Sphere(1e5, Point{ -1e5+99,40.8,81.6 }, Color::white()) ); //Right
    S.addSphere( Sphere(1e5, Point{ 50,40.8, 1e5 }, Color::white()) ); //Back
    S.addSphere( Sphere(1e5, Point{ 50, 1e5, 81.6 }, Color::white()) ); //Bottom
    S.addSphere( Sphere(1e5, Point{ 50,-1e5+81.6,81.6 }, Color::white()) ); //Top

    // Spheres
    S.addSphere( Sphere(16.5, Point{ 27,16.5,47 }, Color(0, 255, 255)) ); //left
    S.addSphere( Sphere(16.5, Point{ 73,16.5,78}, Color(255, 0, 255)) ); //right

    // Lights
    S.addLight( Light(Point{ 0, 0,50 }, 100000));

    return S;
}

Scene very_simple() {
    Scene S = Scene();

    S.addLight( Light(Point{ 200,250,-100 }, 100000));
    S.addLight( Light(Point{ -200,-250, -100 }, 100000));

    S.addSphere( Sphere(200, Point{ 0,0,300 }, Color::white()) );
    S.addSphere( Sphere(150, Point{ -400,-200,320 }, Color(200, 0, 0) ));
    return S;
}

Scene close_scene() {
    Scene S = Scene();

    S.addSphere( Sphere(10, Point{ 0,0,20 }, Color::white()) );

    S.addLight( Light(Point{ 0,0, -100 }, 100) );

    return S;
}

int main()
{
    constexpr int w = 800;
    constexpr int h = 600;

    // We use the ppm format
    std::ofstream fileOut;
    fileOut.open("rtresult.ppm", std::fstream::out);
    fileOut << "P3" << std::endl << std::to_string(w) << " " << std::to_string(h) << std::endl << "255" << std::endl;

    const float focal = 1000.0;

    Scene S = very_simple();

    for (int i = 0; i < h; i++) {
        float y = i;
        for (int j = 0; j < w; j++) {
            float x = j;

            Point pixel = Point{static_cast<float>(x * 2.0 - w),static_cast<float>(y * 2.0 - h),0.0};

            Point focalPoint = Point{0.0,0.0,-focal};
            Direction direction = pixel - focalPoint;

            Rayon ray = Rayon(pixel, direction.normalize());

            if (Intersection it_m = intersect_spheres(S, ray); it_m.isIntersection) {
                // Compute the distance in "scene"-space
                Color v = Color::black();

                for (auto l : S.lights) {
                    Direction to_light = l.position - it_m.intersection;
                    float light_distance = to_light.lenght_squared();

                    Direction N = (it_m.intersection - it_m.sphere.center).normalize();
                    float cos = to_light.normalize().dot(N);

                    Color light_contribution = (it_m.sphere.albedo * (cos / light_distance)) * l.intensity;

                    v = v + light_contribution * visibility(S, l, it_m.intersection);
                }

                v.cap();
                write_color(&fileOut, v);
            }
            else {
                write_color(&fileOut, Color(30, 200, 30));
            }
        }
    }
}