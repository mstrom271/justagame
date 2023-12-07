#pragma once

#include "camera2d.h"
#include "kdtree2d.h"
#include "mesh2d.h"
#include "object2d.h"
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QVector4D>
#include <list>

constexpr std::size_t debug_VBO_number = 2;

class world2d {
    camera2d camera;
    std::list<object2d *> objects;

    QOpenGLBuffer *debug_VBO_array[debug_VBO_number]{nullptr, nullptr};
    QVector4D debug_colors_array[debug_VBO_number]{QVector4D(0, 1, 0, 1),
                                                   QVector4D(1, 1, 1, 1)};

    bBox collisionModel_bBox;
    KDTree2d *kdtree = nullptr;

  public:
    world2d() = default;
    ~world2d() {
        for (size_t i = 0; i < debug_VBO_number; i++) {
            delete debug_VBO_array[i];
            debug_VBO_array[i] = nullptr;
        }
    }

    void add(object2d *object) { objects.push_back(object); }

    auto begin() { return objects.begin(); }
    auto end() { return objects.end(); }

    void setCamera(const camera2d &newCamera) { camera = newCamera; }
    camera2d getCamera() { return camera; }

    void precalc(bool isDebug = false) {
        for (auto debug_VBO : debug_VBO_array) {
            delete debug_VBO;
            debug_VBO = nullptr;
        }

        std::vector<float> vertices[debug_VBO_number];

        bool collisionModelBBox_init = false;
        for (auto object : *this) {
            object->precalcCollisionModel();
            if (isDebug)
                object->precalcDebug_VBO(vertices[1]);
            object->precalcDisplayModel();

            // bBox
            if (collisionModelBBox_init)
                collisionModel_bBox += object->getBBox();
            else {
                collisionModel_bBox = object->getBBox();
                collisionModelBBox_init = true;
            }
        }

        delete kdtree;
        kdtree = new KDTree2d(collisionModel_bBox);
        for (auto object : objects)
            object->precalcCollisionModel_KDTree(kdtree);
        kdtree->collisionDetection();
        kdtree->precalcDebug_VBO(vertices[0]);

        for (std::size_t i = 0; i < debug_VBO_number; i++) {
            debug_VBO_array[i] = new QOpenGLBuffer();
            debug_VBO_array[i]->create();
            debug_VBO_array[i]->bind();
            debug_VBO_array[i]->allocate(vertices[i].size() * sizeof(GLfloat));
            debug_VBO_array[i]->write(0, vertices[i].data(),
                                      vertices[i].size() * sizeof(GLfloat));
            debug_VBO_array[i]->release();
        }
    }

    QOpenGLBuffer *getDebug_VBO(std::size_t index) {
        return debug_VBO_array[index];
    }
    QVector4D getDebug_color(std::size_t index) {
        return debug_colors_array[index];
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
