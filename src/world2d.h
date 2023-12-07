#pragma once

#include "camera2d.h"
#include "kdtree2d.h"
#include "mesh2d.h"
#include "object2d.h"
#include <QDebug>
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
    std::vector<collisionObjectsPoint> collisionPoints;

  public:
    world2d() = default;
    ~world2d();

    void add(object2d *object);
    std::list<object2d *> &getObjects();
    void setCamera(const camera2d &newCamera);
    camera2d getCamera();
    void precalc(bool isDebug = false);
    void collisionDetection();
    void resolveCollisions();
    QOpenGLBuffer *getDebug_VBO(std::size_t index);
    QVector4D getDebug_color(std::size_t index);
    void destroy();
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

// void updateWorld(double sec) {
//     for (auto &obj : objects) {
//         obj.setPos(obj.getPos() + obj.getSpeed() * sec);
//         obj.setAngle(obj.getAngle() + obj.getAngleSpeed() * sec);
//     }
// }
