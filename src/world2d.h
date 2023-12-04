#pragma once

#include "camera2d.h"
#include "mesh2d.h"
#include "object2d.h"
#include <list>

class world2d {
    camera2d camera;
    std::list<object2d *> objects;

  public:
    world2d() = default;

    void add(object2d *object) { objects.push_back(object); }

    auto begin() { return objects.begin(); }
    auto end() { return objects.end(); }

    void setCamera(const camera2d &newCamera) { camera = newCamera; }
    camera2d getCamera() { return camera; }

    void precalc(bool isDebug = false) {
        for (auto object : objects) {
            object->precalcCollisionModel();
            object->precalcVBO(isDebug);
        }
    }

    void destroy() { objects.clear(); }
};

// class world2d {
//     std::list<object2d> objects;
//     std::list<collisionObjectsPoint> collisionPoints;

//   public:
//     ~world2d();

//     void draw(QPainter &painter) const;
//     object2d &add(const object2d &obj);
//     object2d &add(object2d &&obj);

//     void collisionDetection();
//     void updateWorld(double sec);
//     void resolveCollisions();
// };

// world2d::~world2d() {}

// void world2d::draw(QPainter &painter) const {
//     for (const auto &obj : objects)
//         obj.draw(painter);

//     for (const auto &point : collisionPoints)
//         point.draw(painter);
// }

// object2d &world2d::add(const object2d &obj) {
//     objects.push_back(obj);

//     return objects.back();
// }

// object2d &world2d::add(object2d &&obj) {
//     objects.emplace_back(std::move(obj));

//     return objects.back();
// }

// void world2d::collisionDetection() {
//     collisionPoints.clear();
//     collisionObjectsPoint point;
//     for (auto i = std::begin(objects); i != std::end(objects); i++) {
//         auto j = i;
//         j++;
//         for (; j != std::end(objects); j++) {
//             if (i->collisionDetection(*j, point))
//                 collisionPoints.push_back(point);
//         }
//     }
// }

// void world2d::updateWorld(double sec) {
//     for (auto &obj : objects) {
//         obj.setPos(obj.getPos() + obj.getSpeed() * sec);
//         obj.setAngle(obj.getAngle() + obj.getAngleSpeed() * sec);
//     }
// }

// void world2d::resolveCollisions() {
//     for (auto &point : collisionPoints) {
//         point.getObj1()->setPos(point.getObj1()->getPos() +
//                                 point.getNormal2() * point.getDepth() / 2);
//         point.getObj2()->setPos(point.getObj2()->getPos() +
//                                 point.getNormal1() * point.getDepth() / 2);
//     }
//     collisionPoints.clear();
// }
