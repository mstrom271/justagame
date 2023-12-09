#pragma once

#include "math2d.h"
#include "primitive2d.h"
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <list>

class collisionObjectsPoint;
class KDTree2d;

class object2d {
    vec2d pos;
    double angle;

    vec2d speed;
    double angleSpeed;

    double weight = 1;
    bool isFixed = false;

    std::list<primitive2d *> collisionModel; // in local coords

    // precalc collisionModel  values
    mat23 collisionModel_matrix;
    std::list<primitive2d *> collisionModel_precalc; // in world coords
    bBox collisionModel_bBox;
    bool collisionModel_expired = true;

    // precalc displayModel values in object coords
    QOpenGLTexture *displayModel_texture = nullptr;
    QOpenGLBuffer *displayModel_VBO = nullptr;
    bool displayModel_VBO_expired = true;

  public:
    object2d() = default;
    object2d(const object2d &) = delete;
    object2d &operator=(const object2d &) = delete;
    object2d(object2d &&) noexcept = delete;
    object2d &operator=(object2d &&) noexcept = delete;
    ~object2d();

    vec2d getPos() const;
    void setPos(const vec2d &newPos);
    double getAngle() const;
    void setAngle(double newAngle);

    vec2d getSpeed() const;
    void setSpeed(vec2d newSpeed);
    double getAngleSpeed() const;
    void setAngleSpeed(double newAngleSpeed);

    bool getIsFixed() const;
    void setIsFixed(bool newIsFixed);

    double getWeight() const;
    void setWeight(double newWeight);

    void add(primitive2d *p);
    void explosion(vec2d local_point);
    void applyForce(vec2d force, vec2d forcePoint);

    QOpenGLTexture *getDisplayModel_texture();
    QOpenGLBuffer *getDisplayModel_VBO();

    void precalcCollisionModel();

    void precalcDebug_VBO(std::vector<float> &vertices);
    void precalcDisplayModel();

    void precalcCollisionModel_KDTree(KDTree2d *kdtree);
    // void precalcDisplayModel_KDTree(KDTree2d *kdtree);

    bBox getBBox();
};

bool collisionDetection(const object2d &obj1, const object2d &obj2,
                        collisionObjectsPoint &point);

class collisionObjectsPoint {
    object2d *obj1, *obj2;
    vec2d pos;     // absolute coordinate system
    vec2d normal1; // normal from obj1
    vec2d normal2; // normal from obj2
    double depth;  // max depth to obj
  public:
    collisionObjectsPoint(object2d *obj1, object2d *obj2, const vec2d &pos,
                          const vec2d &normal1, const vec2d &normal2,
                          double depth);
    collisionObjectsPoint() = default;
    collisionObjectsPoint(const collisionObjectsPoint &) = default;
    collisionObjectsPoint &operator=(const collisionObjectsPoint &) = default;
    collisionObjectsPoint(collisionObjectsPoint &&) noexcept = default;
    collisionObjectsPoint &
    operator=(collisionObjectsPoint &&) noexcept = default;

    object2d *getObj1() const;
    void setObj1(object2d *newObj1);
    object2d *getObj2() const;
    void setObj2(object2d *newObj2);
    vec2d getPos() const;
    void setPos(const vec2d &newPos);
    vec2d getNormal1() const;
    void setNormal1(const vec2d &newNormal1);
    vec2d getNormal2() const;
    void setNormal2(const vec2d &newNormal2);
    double getDepth() const;
    void setDepth(double newDepth1);
};
