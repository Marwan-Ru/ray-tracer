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

    if(const optional<Intersection> ret = intersectObjectHierarchy(S.root, r); ret.has_value()) {
        if (const float intersection_distance = (ret.value().intersection - p).length(); intersection_distance < (l.position - p).length()) {
            return 0;
        }
    }

    return 1;
}

// --- Some scenes --
/*
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
*/


Scene n_sphere_scene(int n) {
    vector<Sphere> spheres;
    vector<Light> lights;

    const float d = 300.0f / static_cast<float>(n);
    const float radius = 80.f / static_cast<float>(n);

    for (int i = -n; i < n; i++) {
        for (int j = -n; j < n; j++) {
            for (int k = -n; k < n; k++) {
                spheres.emplace_back(radius, Point(static_cast<float>(i) * d, static_cast<float>(j) * d, static_cast<float>(k) * d), Color::white());
            }
        }
    }

    lights.push_back({ { 5000.f, 0.f, 0.f }, 400000.f });
    lights.push_back({ { 1.f, -1000.f, 0.f }, 100000.f});
    lights.push_back({ { -1000.f, 1000.f, 0.f }, 100000.f });
    Scene S = Scene(build_hierarchy(spheres), lights);

    return S;
}

static float buffer[1080][1920][3];
int main()
{
    constexpr int w = 1920;
    constexpr int h = 1080;

    // We use the ppm format
    std::ofstream fileOut;
    fileOut.open("rtresult.ppm", std::fstream::out);
    fileOut << "P3" << std::endl << std::to_string(w) << " " << std::to_string(h) << std::endl << "255" << std::endl;

    int n = 10;
    Scene S =  n_sphere_scene(n); // One million sphere -> n = 50

    cout << 8 * n * n * n << " Spheres in the scene, beginning ray tracing..." << endl;

    clock_t begin = clock();

    for(int a = 0; a < 10; a++) {
        for (int i = 0; i < h; i++) {
            auto y = static_cast<float>(i);
            for (int j = 0; j < w; j++) {
                constexpr float focal = 10000.0;
                auto x = static_cast<float>(j);

                Point pixel = Point{static_cast<float>(x * 2.0 - w),static_cast<float>(y * 2.0 - h),0.0};

                Point focalPoint = Point{0.0,0.0,-focal};
                Direction direction = pixel - focalPoint;

                Ray ray = Ray(pixel, direction.normalize());

                if (optional<Intersection> it_m = intersectObjectHierarchy(S.root, ray); it_m.has_value()) {
                    // Compute the distance in "scene"-space
                    Color v = Color::black();

                    for (auto l : S.lights) {
                        Direction to_light = l.position - it_m.value().intersection;
                        float light_distance = to_light.length_squared();

                        Direction N = (it_m.value().intersection - it_m.value().sphere->center).normalize();
                        float cos = to_light.normalize().dot(N);

                        Color light_contribution = (it_m.value().sphere->albedo * (cos / light_distance)) * l.intensity;

                        v = v + light_contribution * visibility(S, l, it_m.value().intersection);
                    }
                    v.cap();
                    buffer[i][j][0] = v.red;
                    buffer[i][j][1] = v.green;
                    buffer[i][j][2] = v.blue;
                    //write_color(&fileOut, v);
                }
                else {
                    buffer[i][j][0] = 40.0f;
                    buffer[i][j][1] = 40.0f;
                    buffer[i][j][2] = 40.0f;
                }
            }
        }
    }
    clock_t end = clock();

    cout << "Mean Time elapsed in ms: " << (end - begin) * 1000.0 / CLOCKS_PER_SEC / 10 << std::endl;

    for (auto & i : buffer) {
        for (auto & j : i) {
            write_color(&fileOut, j);
        }
    }

    fileOut.close();
}
