#pragma once

#include "camera2d.h"
#include "connection2d.h"
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
    std::list<connection2d *> connections;

    QOpenGLBuffer *debug_VBO_array[debug_VBO_number]{nullptr, nullptr};
    QVector4D debug_colors_array[debug_VBO_number]{QVector4D(0, 1, 0, 1),
                                                   QVector4D(1, 1, 1, 1)};

    bBox collisionModel_bBox;
    KDTree2d *kdtree = nullptr;
    std::vector<collisionObjectsPoint> collisionPoints;

  public:
    world2d() = default;
    ~world2d();

    void addObject(object2d *object);
    void deleteObject(object2d *object);
    void addConnection(connection2d *connection);
    void deleteConnection(connection2d *connection);
    std::list<object2d *> &getObjects();
    void setCamera(const camera2d &newCamera);
    camera2d getCamera();
    void precalc(bool isDebug = false);
    void collisionDetection();
    void collisionResolve();
    void intersect(const primitive2d &primitive, std::vector<Item> &result);
    void intersect(const vec2d &point, std::vector<Item> &result);
    void update(double sec);
    QOpenGLBuffer *getDebug_VBO(std::size_t index);
    QVector4D getDebug_color(std::size_t index);
    void destroy();
};