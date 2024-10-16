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

    Sphere(const float &radius, const Point &center, const Color &albedo) : radius(radius), center(center), albedo(albedo) {}
};

struct Light {
    Point position;
    float intensity;

    Light(const Point &position, const float &intensity) : position(position), intensity(intensity) {}
};


struct Intersection {
    float t;
    Point intersection;
    const Sphere * sphere;

    Intersection(const float &t, const Point &intersection, const Sphere * sphere) : t(t), intersection(intersection), sphere(sphere) {}
};

inline std::optional<Intersection> intersect_sphere(const Sphere &s, const Ray &r) {
    float t = -INFINITY;
    const Direction oc = r.origin - s.center;

    // Note : we can simplify the value of a if ray direction is normalized
    const float a = r.direction.length_squared();
    const float b = 2.0f * oc.dot(r.direction);
    const float c = oc.length_squared() - sq(s.radius);

    // is intersection (we don't care yet about positive or first)
    if (const float delta = b * b - 4.0f * a * c; delta >= 0.0f) {
        const float sqdelta = sqrt(delta);
        const float t1 = (-b - sqdelta) / (2.0f * a);
        const float t2 = (-b + sqdelta) / (2.0f * a);

        if (t1 >= 0.0f && t2 >= 0.0f) {
            t = min(t1, t2); // Choose the closer intersection
        } else if (t1 >= 0.0f) {
            t = t1;
        }
        else if (t2 >= 0.0f) {
            t = t2;
        }

        if(t >= 0.0) {
            return Intersection( t, r.origin + r.direction * t, &s);
        }else {
            return nullopt;
        }

    }
    return nullopt;
}

inline std::optional<Intersection> intersect_spheres(const vector<Sphere>& spheres, const Ray &r) {
    float closest = INFINITY;
    std::optional<Intersection> closest_intersection = nullopt;
    for (auto sphere : spheres) {
        if (std::optional<Intersection> int_i = intersect_sphere(sphere, r); int_i.has_value()) {
            if(int_i.value().t < closest && int_i.value().t >= 0) {
                closest = int_i.value().t;
                closest_intersection = int_i;
            }
        }

    }
    return closest_intersection;
}

// Object hierarchy

inline AABB sphere_to_aabb(const Sphere &sphere){
    const Point pmin = Point{sphere.center.x - sphere.radius, sphere.center.y - sphere.radius, sphere.center.z - sphere.radius};
    const Point pmax = Point{sphere.center.x + sphere.radius, sphere.center.y + sphere.radius, sphere.center.z + sphere.radius};

    return AABB{pmin, pmax};
}

class ObjectHierarchy {
public:
    bool isLeaf;
    AABB aabb;
    vector<Sphere> spheres;
    vector<ObjectHierarchy> childs;

    ObjectHierarchy(const AABB &aabb, const vector<Sphere> &spheres) : isLeaf(true), aabb(aabb), spheres(spheres) {}
    ObjectHierarchy(const AABB &aabb, const vector<ObjectHierarchy> &childs): isLeaf(false), aabb(aabb), childs(childs) {}

};

// Comparison functions
inline bool compare_sphere_X(const Sphere &s1, const Sphere &s2) {
    return s1.center.x < s2.center.x;
}
inline bool compare_sphere_Y(const Sphere &s1, const Sphere &s2) {
    return s1.center.y < s2.center.y;
}
inline bool compare_sphere_Z(const Sphere &s1, const Sphere &s2) {
    return s1.center.z < s2.center.z;
}

inline ObjectHierarchy build_hierarchy(vector<Sphere> &spheres) {

    // Creating the smallest bounding box that contains all the spheres
    AABB aabb = sphere_to_aabb(spheres[0]);
    for (auto sphere : spheres) {
        aabb = aabb.unionAABB(sphere_to_aabb(sphere));
    }

    if (spheres.size() < LEAF_SIZE){
        return {aabb, spheres}; // Leaf
    } else {
        switch (aabb.largestAxis()) {
            case Axis::X:
                sort(spheres.begin(), spheres.end(), compare_sphere_X);
            case Axis::Y:
                sort(spheres.begin(), spheres.end(), compare_sphere_Y);
            case Axis::Z:
                sort(spheres.begin(), spheres.end(), compare_sphere_Z);
        }
    }

    vector<Sphere> left_spheres;
    vector<Sphere> right_spheres;
    const int cut = spheres.size() / 2;

    left_spheres.reserve(cut);
    for(int i = 0; i < cut; i++) {
        left_spheres.push_back(spheres[i]);
    }
    right_spheres.reserve(spheres.size() - cut);
    for(int i = cut; i < spheres.size(); i++) {
        right_spheres.push_back(spheres[i]);
    }

    vector<ObjectHierarchy> childs;
    childs.push_back(build_hierarchy(left_spheres));
    childs.push_back(build_hierarchy(right_spheres));

    return {aabb, childs}; // Node
}

inline std::optional<Intersection> intersectObjectHierarchy(const ObjectHierarchy &obj, const Ray &ray) {
    if(!intersect_aabb(obj.aabb, ray).has_value()) {
        return nullopt;
    }
    if(obj.isLeaf) {
        return intersect_spheres(obj.spheres, ray);
    }


    std::optional<Intersection> itleft = intersectObjectHierarchy(obj.childs[0], ray);
    std::optional<Intersection> itright = intersectObjectHierarchy(obj.childs[1], ray);

    if(itleft.has_value()) {
        if(itright.has_value()) { // Both aabb intersects
            // So we take the closest one
            if ( itright.value().t < itleft.value().t) return itright;
        }
        return itleft;
    }
    return itright;
}

struct Scene {
    ObjectHierarchy root;
    vector<Light> lights;

    Scene(ObjectHierarchy root, const vector<Light>& lights) : root(std::move(root)), lights(lights) {}

    void addLight(const Light& light) {
        lights.push_back(light);
    }
};

#endif //INTERSECTION_H
