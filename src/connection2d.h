#pragma once

#include "math2d.h"
#include "object2d.h"

class connection2d {
    object2d *obj1 = nullptr, *obj2 = nullptr;
    vec2d point1, point2; // point on object in local coord. If obj==nullptr,
                          // then fixed point in world coord
  public:
    connection2d() = default;
    connection2d(object2d *obj1, object2d *obj2, vec2d point1, vec2d point2)
        : obj1(obj1), obj2(obj2), point1(point1), point2(point2) {}

    void setObject1(object2d *newObj1) { obj1 = newObj1; }
    void setObject2(object2d *newObj2) { obj2 = newObj2; }
    void setPoint1(vec2d newPoint1) { point1 = newPoint1; }
    void setPoint2(vec2d newPoint2) { point2 = newPoint2; }

    object2d *getObject1() { return obj1; }
    object2d *getObject2() { return obj2; }
    vec2d getPoint1() { return point1; }
    vec2d getPoint2() { return point2; }

    void precalcDebug_VBO(std::vector<float> &vertices) {
        vec2d worldPos1{0, 0};
        vec2d worldPos2{0, 0};
        if (obj1)
            worldPos1 = obj1->getPos();
        if (obj2)
            worldPos2 = obj2->getPos();

        vertices.push_back((point1 + worldPos1).x());
        vertices.push_back((point1 + worldPos1).y());
        vertices.push_back((point2 + worldPos2).x());
        vertices.push_back((point2 + worldPos2).y());
    }
};