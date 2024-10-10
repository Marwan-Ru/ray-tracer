// Ray-tracer.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//

// ReSharper disable CppUseAuto
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>

#include "util.h"
#include "ray.h"
#include "intersection.h"
#include "tests.h"

using namespace std;

/*Calculates light visibility for a given light and point*/
float visibility(const Scene& S, const Light l, const Point p) {
    const Direction dir = (l.position - p).normalize();
    const auto r = Ray(p + dir * 0.1, dir);

    const float light_distance = (l.position - p).length();

    if(const Intersection ret = intersect_spheres(S.spheres, r); ret.isIntersection) {
        if (const float intersection_distance = (ret.intersection - p).length(); intersection_distance < light_distance) {
            return 0;
        }
    }

    return 1;
}

// --- Some scenes --
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

Scene million_sphere_scene() {
    Scene S = Scene();

    for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 100; j++) {
            for (int k = 0; k < 100; k++) {
                S.addSphere(Sphere(1, Point(i * 10,j* 10,k* 10), Color::white()));
            }
        }
    }

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

    Scene S = full_test_scene();

    for (int i = 0; i < h; i++) {
        float y = i;
        for (int j = 0; j < w; j++) {
            float x = j;

            Point pixel = Point{static_cast<float>(x * 2.0 - w),static_cast<float>(y * 2.0 - h),0.0};

            Point focalPoint = Point{0.0,0.0,-focal};
            Direction direction = pixel - focalPoint;

            Ray ray = Ray(pixel, direction.normalize());

            if (Intersection it_m = intersect_spheres(S.spheres, ray); it_m.isIntersection) {
                // Compute the distance in "scene"-space
                Color v = Color::black();

                for (auto l : S.lights) {
                    Direction to_light = l.position - it_m.intersection;
                    float light_distance = to_light.length_squared();

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