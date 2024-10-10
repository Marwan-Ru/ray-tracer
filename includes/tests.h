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

// --- Intersections related tests ---

inline bool test_sphere_intersection() {
    const Sphere s1 = Sphere(10, Point(0,0,-20), Color(1,1,1));

    const Ray r1 = Ray(Point(0,0,0), Direction(0,0,-1)); // This one intersects
    const Ray r2 = Ray(Point(0,0,0), Direction(0,0,1)); // This one doesn't
    const Ray r3 = Ray(Point(0,0,-20), Direction(0,0,-1)); // This one is inside and should intersect
    const Ray r4 = Ray(Point(0,20,-20), Direction(0,0,-11)); // This one doesn't

    return intersect_sphere(s1, r1).isIntersection && !(intersect_sphere(s1, r2).isIntersection) && intersect_sphere(s1, r3).isIntersection && !intersect_sphere(s1, r4).isIntersection;
}

inline bool test_aabb_intersection() {
    const AABB c1 = AABB(Point(10,10,10), Point(20,20,20));

    const Ray r1 = Ray(Point(-20,10,10), Direction(1,0,0)); // Should intersect
    const Ray r2 = Ray(Point(-20,-10,-10), Direction(-1,0,0)); // Should not intersect
    const Ray r3 = Ray(Point(-20,15,15), Direction(1,0,0)); // Should intersect
    const Ray r4 = Ray(Point(15,-15,15), Direction(0,1,0)); // Should intersect
    const Ray r5 = Ray(Point(-20,-20,0), Direction(1,0,0)); // Should not intersect
    const Ray r6 = Ray(Point(15,15,-15), Direction(0,0,1)); // Should intersect
    const Ray r7 = Ray(Point(15,15,15), Direction(0,0,1)); // Should intersect

    const InterAABB res1 = intersect_aabb(c1, r1);
    const InterAABB res2 = intersect_aabb(c1, r2);
    const InterAABB res3 = intersect_aabb(c1, r3);
    const InterAABB res4 = intersect_aabb(c1, r4);
    const InterAABB res5 = intersect_aabb(c1, r5);
    const InterAABB res6 = intersect_aabb(c1, r6);
    const InterAABB res7 = intersect_aabb(c1, r7);


    return res1.isIntersection && !res2.isIntersection && res3.isIntersection && res4.isIntersection && !res5.isIntersection && res6.isIntersection && res7.isIntersection;
}

inline bool test_optimised_aabb_intersection() {
    const AABB c1 = AABB(Point(10,10,10), Point(20,20,20));

    const Ray r1 = Ray(Point(15,15,15), Direction(0,0,1)); // Ray inside box should intersect
    const Ray r2 = Ray(Point(-15,15,15), Direction(-1,0,0)); // Box behind ray origin so should not intersect (if optimized)

    const InterAABB res1 = intersect_aabb(c1, r1);
    const InterAABB res2 = intersect_aabb(c1, r2);

    return res1.isIntersection && !res2.isIntersection;
}

// --- end intersection related tests ---

// --- Object Hierarchy related tests ---

inline bool test_sphere_to_aabb() {
    const Sphere s1 = Sphere(10, Point(0,0,0), Color::white());
    const Sphere s2 = Sphere(20, Point(-100,0,0), Color::white());
    const Sphere s3 = Sphere(1, Point(100, 100, 100), Color::white());

    const AABB aabb1 = sphere_to_aabb(s1);
    const AABB aabb2 = sphere_to_aabb(s2);
    const AABB aabb3 = sphere_to_aabb(s3);

    const bool res1 = aabb1.pmin == Point(-10,-10,-10) && aabb1.pmax == Point(10,10,10);
    const bool res2 = aabb2.pmin == Point(-120, -20, -20) && aabb2.pmax == Point(-80, 20, 20);
    const bool res3 = aabb3.pmin == Point(99, 99, 99) && aabb3.pmax == Point(101, 101, 101);

    return res1 && res2 && res3;
}

inline bool test_union_aabb() {
    const AABB aabb1 = AABB(Point(10,10,10), Point(20,20,20));
    const AABB aabb2 = AABB(Point(15,15,15), Point(15,15,15));
    const AABB aabb3 = AABB(Point(-15,-15,-15), Point(13.3,14.2,23.2));

    const AABB u1 = aabb1.unionAABB(aabb2);
    const AABB u2 = aabb2.unionAABB(aabb3);

    return u1.pmin == Point(10,10,10) && u1.pmax == Point(20,20,20) && u2.pmin == Point(-15,-15,-15) && u2.pmax == Point(15,15,23.2);
}

inline bool test_BVH_creation() {
    vector<Sphere> spheres;

    for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 100; j++) {
            for (int k = 0; k < 100; k++) {
                spheres.emplace_back(1, Point(i * 10,j* 10,k* 10), Color::white());
            }
        }
    }

    ObjectHierarchy BVH = build_hierarchy(spheres);

    return BVH.aabb.pmax.z == 991 && BVH.aabb.pmin.z == -1 && BVH.childs[0].aabb.pmax.z == 491 && BVH.childs[0].aabb.pmin.z == -1 && BVH.childs[1].aabb.pmax.z == 991 && BVH.childs[1].aabb.pmin.z == 499;
}

inline bool test_simple_BVH_to_ray() {
    vector<Sphere> spheres;
    spheres.emplace_back(0, Point(0,0,-20), Color::white());

    const Ray r1 = Ray(Point(0,0,0), Direction(0,0,-1)); // This one intersects

    const ObjectHierarchy BVH = build_hierarchy(spheres);

    const Intersection it1 = intersectObjectHierarchy(BVH, r1);

    return it1.isIntersection;
}

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

    cout << "Testing full test scene ... " << endl << endl;

    cout << "--- RAY INTERSECTION ---" << endl;
    launch_test("Ray initialisation", test_ray_init());
    launch_test("Ray sphere intersection", test_sphere_intersection());
    launch_test("Regular Ray AABB intersection" , test_aabb_intersection());
    launch_test("Optimized Ray AABB intersection", test_optimised_aabb_intersection());

    cout << endl << "--- BVH ---" << endl;

    launch_test("Creating AABB of a sphere", test_sphere_to_aabb());
    launch_test("AABB Union", test_union_aabb());
    //launch_test("BVH creation", test_BVH_creation());
    launch_test("Simple ray intersection using BVH", test_simple_BVH_to_ray());

    // --- End of unit testing ---

    // We create a simple scene and pass it to test that we get an image
    Scene S;

    S.addLight( Light(Point{ 200,250,-100 }, 100000));

    S.addSphere( Sphere(200, Point{ 0,0,300 }, Color::white()) );

    return S;
};
#endif //TESTS_H
