#pragma once

#include "math2d.h"

class bBox {
    bool isInit = false;

    vec2d pos; // center
    double radius;

  public:
    bBox() = default;
    bBox(const bBox &rect) = default;
    bBox &operator=(const bBox &rect) = default;
    bBox(bBox &&rect) noexcept = default;
    bBox &operator=(bBox &&rect) noexcept = default;
    ~bBox() = default;

    bBox(const vec2d &pos, double radius);
    bBox(double x, double y, double radius);
    vec2d getPos() const;
    void setPos(const vec2d &newPos);
    double getRadius() const;
    void setRadius(double newRadius);
    bool getIsInit() const;
    void setIsInit(bool newIsInit);

    bBox operator+(const bBox &other) const;
    bBox &operator+=(const bBox &other);

    bool intersect(const bBox &other) const;
};

class collisionPrimitivesPoint {
    vec2d pos;     // absolute coordinate system
    vec2d normal1; // normal from primitive1
    vec2d normal2; // normal from primitive2
    double depth;  // penetration depth
  public:
    collisionPrimitivesPoint() = default;
    collisionPrimitivesPoint(const vec2d &pos, const vec2d &normal1,
                             const vec2d &normal2, double depth);
    const vec2d &getPos() const;
    void setPos(const vec2d &newPos);
    const vec2d &getNormal1() const;
    void setNormal1(const vec2d &newNormal);
    const vec2d &getNormal2() const;
    void setNormal2(const vec2d &newNormal);
    double getDepth() const;
    void setDepth(double newDepth);
    void swap();
};

class primitive2d {
  public:
    virtual ~primitive2d() = default;

    virtual void precalc(const mat23 &matrix) = 0;
    virtual primitive2d *clone() const = 0;
    virtual bBox getBBox() const = 0;
};

class circle2d : public primitive2d {
    vec2d pos;
    double radius;

  public:
    circle2d(const vec2d &pos, double radius);

    vec2d getPos() const;
    void setPos(const vec2d &newPos);
    double getRadius() const;
    void setRadius(double newRadius);

    void precalc(const mat23 &matrix) override;
    primitive2d *clone() const override;
    bBox getBBox() const override;
};

class line2d : public primitive2d {
    vec2d p1, p2;

  public:
    line2d(const vec2d &p1, const vec2d &p2);

    const vec2d &getP1() const;
    void setP1(const vec2d &newP1);
    const vec2d &getP2() const;
    void setP2(const vec2d &newP2);

    void precalc(const mat23 &matrix) override;
    primitive2d *clone() const override;
    bBox getBBox() const override;
};

class rectangle2d : public primitive2d {
    vec2d pos;
    vec2d size;
    double angle;

    vec2d p1, p2, p3, p4;

  public:
    rectangle2d(const vec2d &pos, const vec2d &size, double angle);

    const vec2d &getPos() const;
    void setPos(const vec2d &newPos);
    const vec2d &getSize() const;
    void setSize(const vec2d &newSize);
    double getAngle() const;
    void setAngle(double newAngle);

    const vec2d &P1() const;
    void setP1(const vec2d &newP1);
    const vec2d &P2() const;
    void setP2(const vec2d &newP2);
    const vec2d &P3() const;
    void setP3(const vec2d &newP3);
    const vec2d &P4() const;
    void setP4(const vec2d &newP4);

    void precalc(const mat23 &matrix) override;
    primitive2d *clone() const override;
    bBox getBBox() const override;
};

bool collisionPrimitives(const circle2d &c1, const circle2d &c2,
                         collisionPrimitivesPoint &point);
bool collisionPrimitives(const circle2d &c1, const line2d &l2,
                         collisionPrimitivesPoint &point);
bool collisionPrimitives(const circle2d &c1, const rectangle2d &r2,
                         collisionPrimitivesPoint &point);

bool collisionPrimitives(const line2d &l1, const circle2d &c2,
                         collisionPrimitivesPoint &point);
bool collisionPrimitives(const line2d &l1, const line2d &l2,
                         collisionPrimitivesPoint &point);
bool collisionPrimitives(const line2d &l1, const rectangle2d &r2,
                         collisionPrimitivesPoint &point);

bool collisionPrimitives(const rectangle2d &r1, const circle2d &c2,
                         collisionPrimitivesPoint &point);
bool collisionPrimitives(const rectangle2d &r1, const line2d &l2,
                         collisionPrimitivesPoint &point);
bool collisionPrimitives(const rectangle2d &r1, const rectangle2d &r2,
                         collisionPrimitivesPoint &point);

bool collisionPrimitives(const primitive2d &p1, const primitive2d &p2,
                         collisionPrimitivesPoint &point);