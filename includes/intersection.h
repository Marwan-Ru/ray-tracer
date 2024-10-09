//
// Created by Marwan Ait Addi on 07/10/2024.
//

#ifndef INTERSECTION_H
#define INTERSECTION_H

#include <vector>

#include "ray.h"
#include "util.h"
#include "AABB.h"

using namespace std;

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

inline Intersection intersect_sphere(const Sphere &s, const Ray &r) {
    Intersection ret = Intersection();
    const Direction oc = r.origin - s.center;

    // Note : we can simplify the value of a if ray direction is normalized
    const double a = r.direction.length_squared();
    const double b = 2.0 * oc.dot(r.direction);
    const double c = oc.length_squared() - sq(s.radius);

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

inline Intersection intersect_spheres(const Scene& s, const Ray &r) {
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

// Object hierarchy

inline AABB sphere_to_aabb(const Sphere &sphere)
{
    const Point r = Point{sphere.radius, sphere.radius, sphere.radius};
    const Point pmin = Point{sphere.center.x - sphere.radius, sphere.center.y - sphere.radius, sphere.center.z - sphere.radius};
    const Point pmax = Point{sphere.center.x + sphere.radius, sphere.center.y + sphere.radius, sphere.center.z + sphere.radius};

    return AABB{pmin, pmax};
}


#endif //INTERSECTION_H
