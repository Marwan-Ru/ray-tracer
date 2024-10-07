//
// Created by maaitaddi on 07/10/2024.
//

#ifndef CUBE_H
#define CUBE_H

#include "ray.h"

enum Axis { X, Y, Z };

struct  InterAABB {
  bool isIntersection;
  float tmin;

  InterAABB(const bool isIntersection, const float tmin) : isIntersection(isIntersection), tmin(tmin) {}

  InterAABB() : isIntersection(false), tmin(INFINITY) {}
};

class AABB {
  public:
    Point pmin;
    Point pmax;

    AABB(Point pmin, Point pmax) : pmin(pmin), pmax(pmax) {}

    AABB unionAABB(const AABB &other) {
      return AABB(this->pmin.minp(other.pmin), this->pmax.minp(other.pmax));
    };

    Axis largestAxis() const {
      Direction axis = this->pmax - this->pmin;
      if(axis.x >= axis.y && axis.x >= axis.z) {
        return X;
      }else if (axis.y >= axis.z){
        return Y;
      }else return Z;
    }

    InterAABB intersect(const Ray &ray);
};

#endif //CUBE_H
