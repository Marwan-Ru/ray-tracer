//
// Created by maaitaddi on 07/10/2024.
//

#ifndef AABB_H
#define AABB_H

#include "ray.h"

enum Axis { X, Y, Z };

struct  InterAABB {
  bool isIntersection;
  float tmin;

  InterAABB(const bool isIntersection, const float tmin) : isIntersection(isIntersection), tmin(tmin) {}

  InterAABB() : isIntersection(false), tmin(INFINITY) {}
};

struct  AABB {
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

    InterAABB intersect(const Ray &ray) {
      InterAABB ret = InterAABB();

      // X slab
      float tx1 = (this->pmin.x - ray.origin.x) * ray.inv_direction.x;
      float tx2 = (this->pmax.x - ray.origin.x) * ray.inv_direction.x;

      float tmin = min(tx1, tx2);
      float tmax = max(tx1, tx2);

      // Y slab
      float ty1 = (this->pmin.y - ray.origin.y) * ray.inv_direction.y;
      float ty2 = (this->pmax.y - ray.origin.y) * ray.inv_direction.y;

      float tminp = min(tmin, min(ty1, ty2));
      float tmaxp = max(tmax, max(ty1, ty2));

      // Z slab
      float tz1 = (this->pmin.z - ray.origin.z) * ray.inv_direction.z;
      float tz2 = (this->pmax.z - ray.origin.z) * ray.inv_direction.z;

      float tminpp = min(tminp, min(tz1, tz2));
      float tmaxpp = max(tmaxp, max(tz1, tz2));

      if(tminpp > tmaxpp){
        ret.isIntersection = false;
      }else{
        ret.isIntersection = true;
        ret.tmin = tminpp;
      }
      return ret;
    }
};

#endif //AABB_H
