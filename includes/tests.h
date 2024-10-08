//
// Created by Marwan Ait Addi on 08/10/2024.
//

// ReSharper disable CppUseAuto
#ifndef TESTS_H
#define TESTS_H

#include <cfloat>
#include <string>

#include "util.h"
#include "ray.h"
#include "AABB.h"
#include "intersection.h"

inline bool test_ray_init() {
    const Ray r = Ray(Point(0,0,0), Direction(1,0,0));

    return r.direction == Direction(1,0,0) && r.origin == Point(0,0,0);
}

inline bool test_sphere_to_aabb() {
    const Sphere s1 = Sphere(10, Point(0,0,0), Color::white());
    const Sphere s2 = Sphere(20, Point(-100,0,0), Color::white());

    const AABB aabb1 = sphere_to_aabb(s1);
    const AABB aabb2 = sphere_to_aabb(s2);

    const bool res1 = aabb1.pmin == Point(-10,-10,-10) && aabb1.pmax == Point(10,10,10);
    const bool res2 = aabb2.pmin == Point(-120, -20, -20) && aabb2.pmax == Point(-80, 20, 20);

    return res1 && res2;
}

// --- Intersections related tests ---

inline bool test_sphere_intersection() {
    const Sphere s1 = Sphere(10, Point(0,0,-20), Color(1,1,1));

    const Ray r1 = Ray(Point(0,0,0), Direction(0,0,-1)); // This one intersects
    const Ray r2 = Ray(Point(0,0,0), Direction(0,0,1)); // This one doesn't
    const Ray r3 = Ray(Point(0,0,-20), Direction(0,0,-1)); // This one is inside and should intersect

    return intersect_sphere(s1, r1).isIntersection && !(intersect_sphere(s1, r2).isIntersection) && intersect_sphere(s1, r3).isIntersection;
}

inline bool test_aabb_intersection() {
    const AABB c1 = AABB(Point(0,0,0), Point(1,1,1));

    const Ray r1 = Ray(Point(-20,0,0), Direction(1,0,0)); // Should intersect
    const Ray r2 = Ray(Point(-20,0,0), Direction(-1,0,0)); // Should not intersect
    const Ray r3 = Ray(Point(-20,0,0), Direction(0,-1,0)); // Should not intersect either
    const Ray r4 = Ray(Point(0,0,0), Direction(1,0,0)); // Should intersect
    const Ray r5 = Ray(Point(-20,-20,0), Direction(1,0,0)); // Should not intersect

    const InterAABB res1 = intersect_aabb(c1, r1);
    const InterAABB res2 = intersect_aabb(c1, r2);
    const InterAABB res3 = intersect_aabb(c1, r3);
    const InterAABB res4 = intersect_aabb(c1, r4);
    const InterAABB res5 = intersect_aabb(c1, r5);


    return res1.isIntersection && !res2.isIntersection && !res3.isIntersection && res4.isIntersection && !res5.isIntersection;
}

// --- end intersection related tests ---

inline void launch_test(const string& name, const bool res) {
    cout << "Testing " << name << " ... ";
    if(res) {
        cout << "OK" << endl;
    }else {
        cout << "FAILED" << endl;
    }
}

inline Scene full_test_scene() {
    // --- Actual unit testing here --

    launch_test("Ray initialisation", test_ray_init());
    launch_test("Ray sphere intersection", test_sphere_intersection());
    launch_test("Ray AABB intersection" , test_aabb_intersection());
    launch_test("Sphere to aabb", test_sphere_to_aabb());

    // --- End of unit testing ---

    // We create a scene and pass it to test that too
    Scene S;

    S.addLight( Light(Point{ 200,250,-100 }, 100000));

    S.addSphere( Sphere(200, Point{ 0,0,300 }, Color::white()) );

    return S;
};
#endif //TESTS_H
