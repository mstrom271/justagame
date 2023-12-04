#pragma once

#include "math2d.h"
#include "primitive2d.h"
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <list>

class collisionObjectsPoint;

class object2d {
    vec2d pos;
    double angle;

    vec2d speed;
    double angleSpeed;

    std::list<primitive2d *> collisionModel;
    // precalc collision model  values in world coords
    bool collisionModel_expired = true;
    mat23 collisionModelMatrix;
    std::list<primitive2d *> collisionModel_precalc;
    bBox collisionModelBBox;

    // mesh2d
    // precalc draw values in object coords
    QOpenGLTexture *draw_texture = nullptr;
    QOpenGLBuffer *draw_vbo = nullptr;
    QOpenGLBuffer *draw_vboDebug = nullptr;
    bool draw_expired = true;

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

    void add(primitive2d *p);
    void explosion(vec2d local_point);

    QOpenGLTexture *getTexture();
    QOpenGLBuffer *getVBO();
    QOpenGLBuffer *getVBODebug();
    void precalcCollisionModel();
    void precalcVBO(bool isDebug = false);
    bBox getBBox();
};

bool collisionDetection(const object2d &obj1, const object2d &obj2,
                        collisionObjectsPoint &point);

// fabrics
void makeCircle2d(object2d *obj, const vec2d &pos, double radius);
void makeLine2d(object2d *obj, const vec2d &p1, const vec2d &p2);
void makeRectangle2d(object2d *obj, const vec2d &pos, const vec2d &size,
                     double angle);

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

    void draw(QPainter &painter) const;
};
