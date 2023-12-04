#include "primitive2d.h"
#include <cmath>

// ----------------------
// bBox
// ----------------------

bBox::bBox(const vec2d &pos, double radius)
    : isInit(true), pos(pos), radius(radius) {}
bBox::bBox(double x, double y, double radius)
    : isInit(true), pos(x, y), radius(radius) {}
double bBox::getRadius() const { return radius; }
void bBox::setRadius(double newRadius) { radius = newRadius; }
vec2d bBox::getPos() const { return pos; }
void bBox::setPos(const vec2d &newPos) { pos = newPos; }
bool bBox::getIsInit() const { return isInit; }
void bBox::setIsInit(bool newIsInit) { isInit = newIsInit; }

bBox bBox::operator+(const bBox &other) const {
    if (!isInit && other.isInit)
        return other;
    else if (!other.isInit)
        return *this;

    double left = std::min(pos.x() - radius, other.pos.x() - other.radius);
    double top = std::min(pos.y() - radius, other.pos.y() - other.radius);

    double right = std::max(pos.x() + radius, other.pos.x() + other.radius);
    double bottom = std::max(pos.y() + radius, other.pos.y() + other.radius);

    vec2d half_diff((right - left) / 2, (bottom - top) / 2);
    return bBox(vec2d(left, top) + half_diff,
                std::max(half_diff.x(), half_diff.y()));
}

bBox &bBox::operator+=(const bBox &other) {
    if (!isInit && other.isInit) {
        *this = other;
        return *this;
    } else if (!other.isInit)
        return *this;

    double left = std::min(pos.x() - radius, other.pos.x() - other.radius);
    double top = std::min(pos.y() - radius, other.pos.y() - other.radius);

    double right = std::max(pos.x() + radius, other.pos.x() + other.radius);
    double bottom = std::max(pos.y() + radius, other.pos.y() + other.radius);

    vec2d half_diff((right - left) / 2, (bottom - top) / 2);
    *this = bBox(vec2d(left, top) + half_diff,
                 std::max(half_diff.x(), half_diff.y()));

    return *this;
}

bool bBox::intersect(const bBox &other) const {
    if (isInit && other.isInit &&
        std::abs(pos.x() - other.pos.x()) <= radius + other.radius &&
        std::abs(pos.y() - other.pos.y()) <= radius + other.radius)
        return true;

    return false;
}

// ----------------------
// circle2d
// ----------------------

circle2d::circle2d(const vec2d &pos, double radius)
    : pos(pos), radius(radius) {}

vec2d circle2d::getPos() const { return pos; }
void circle2d::setPos(const vec2d &newPos) { pos = newPos; }
double circle2d::getRadius() const { return radius; }
void circle2d::setRadius(double newRadius) { radius = newRadius; }

void circle2d::precalc(const mat23 &matrix) { pos *= matrix; }

primitive2d *circle2d::clone() const { return new circle2d(*this); }

bBox circle2d::getBBox() const { return bBox(pos, radius); }

// ----------------------
// line2d
// ----------------------

line2d::line2d(const vec2d &p1, const vec2d &p2) : p1(p1), p2(p2) {}

const vec2d &line2d::getP1() const { return p1; }
void line2d::setP1(const vec2d &newP1) { p1 = newP1; }
const vec2d &line2d::getP2() const { return p2; }
void line2d::setP2(const vec2d &newP2) { p2 = newP2; }

void line2d::precalc(const mat23 &matrix) {
    p1 *= matrix;
    p2 *= matrix;
}

primitive2d *line2d::clone() const { return new line2d(*this); }

bBox line2d::getBBox() const {
    vec2d half_diff((p2 - p1) / 2);
    return bBox(half_diff + p1,
                std::max(std::abs(half_diff.x()), std::abs(half_diff.y())));
}

// ----------------------
// rectangle2d
// ----------------------

rectangle2d::rectangle2d(const vec2d &pos, const vec2d &size, double angle)
    : pos(pos), size(size), angle(angle) {}

const vec2d &rectangle2d::getPos() const { return pos; }
void rectangle2d::setPos(const vec2d &newPos) { pos = newPos; }
const vec2d &rectangle2d::getSize() const { return size; }
void rectangle2d::setSize(const vec2d &newSize) { size = newSize; }
double rectangle2d::getAngle() const { return angle; }
void rectangle2d::setAngle(double newAngle) { angle = newAngle; }

const vec2d &rectangle2d::P1() const { return p1; }
void rectangle2d::setP1(const vec2d &newP1) { p1 = newP1; }
const vec2d &rectangle2d::P2() const { return p2; }
void rectangle2d::setP2(const vec2d &newP2) { p2 = newP2; }
const vec2d &rectangle2d::P3() const { return p3; }
void rectangle2d::setP3(const vec2d &newP3) { p3 = newP3; }
const vec2d &rectangle2d::P4() const { return p4; }
void rectangle2d::setP4(const vec2d &newP4) { p4 = newP4; }

void rectangle2d::precalc(const mat23 &matrix) {
    p1 = vec2d(+size.x() / 2, +size.y() / 2);
    p2 = vec2d(+size.x() / 2, -size.y() / 2);
    p3 = vec2d(-size.x() / 2, -size.y() / 2);
    p4 = vec2d(-size.x() / 2, +size.y() / 2);

    mat23 rectM;
    rectM.rotate(angle);
    rectM.translate(pos);

    rectM *= matrix;

    p1 *= rectM;
    p2 *= rectM;
    p3 *= rectM;
    p4 *= rectM;
}

primitive2d *rectangle2d::clone() const { return new rectangle2d(*this); }

bBox rectangle2d::getBBox() const {
    auto rangeX = {p1.x(), p2.x(), p3.x(), p4.x()};
    auto [left, right] =
        std::minmax_element(std::begin(rangeX), std::end(rangeX));

    auto rangeY = {p1.y(), p2.y(), p3.y(), p4.y()};
    auto [top, bottom] =
        std::minmax_element(std::begin(rangeY), std::end(rangeY));

    return bBox(vec2d((*left + *right) / 2, (*top + *bottom) / 2),
                std::max(*right - *left, *bottom - *top) / 2);
}

// ----------------------
// collisionPrimitivesPoint
// ----------------------

collisionPrimitivesPoint::collisionPrimitivesPoint(const vec2d &pos,
                                                   const vec2d &normal1,
                                                   const vec2d &normal2,
                                                   double depth)
    : pos(pos), normal1(normal1), normal2(normal2), depth(depth) {}
const vec2d &collisionPrimitivesPoint::getNormal1() const { return normal1; }
void collisionPrimitivesPoint::setNormal1(const vec2d &newNormal1) {
    normal1 = newNormal1;
}
const vec2d &collisionPrimitivesPoint::getNormal2() const { return normal2; }
void collisionPrimitivesPoint::setNormal2(const vec2d &newNormal2) {
    normal2 = newNormal2;
}
double collisionPrimitivesPoint::getDepth() const { return depth; }
void collisionPrimitivesPoint::setDepth(double newDepth) { depth = newDepth; }
const vec2d &collisionPrimitivesPoint::getPos() const { return pos; }
void collisionPrimitivesPoint::setPos(const vec2d &newPos) { pos = newPos; }
void collisionPrimitivesPoint::swap() { std::swap(normal1, normal2); }

// ----------------------
// collisionPrimitives
// ----------------------

bool collisionPrimitives(const circle2d &c1, const circle2d &c2,
                         collisionPrimitivesPoint &point) {
    vec2d dist = c2.getPos() - c1.getPos();
    double distLength2 = dist.length2();
    double radius = c2.getRadius() + c1.getRadius();
    double radius2 = radius * radius;
    if (distLength2 > radius2)
        return false;
    else {
        double distLength = std::sqrt(distLength2);
        vec2d normal = dist / distLength;
        double depth = radius - distLength;
        point = collisionPrimitivesPoint(
            c1.getPos() +
                dist * (c1.getRadius() / (c2.getRadius() + c1.getRadius())),
            normal, -normal, depth);
        return true;
    }
}

bool collisionPrimitives(const circle2d &c1, const line2d &l2,
                         collisionPrimitivesPoint &point) {

    // TODO optimizations

    vec2d v1 = c1.getPos() - l2.getP1();
    vec2d v2 = l2.getP2() - l2.getP1();
    double projL = v1.dotProduct(v2) / v2.length();

    if (projL >= 0 && projL <= v2.length()) {
        vec2d v3 = v2 / v2.length() * projL;
        vec2d perp = v3 + l2.getP1() - c1.getPos();
        if (perp.length() <= c1.getRadius()) {
            point.setPos(v3 + l2.getP1());
            point.setDepth(perp.length() - c1.getRadius());
            perp.norm();
            point.setNormal1(perp);
            point.setNormal2(-perp);
            return true;
        }
    } else if (projL < 0 && v1.length() <= c1.getRadius()) {
        point.setPos(l2.getP1());

        vec2d normal1 = (l2.getP1() - c1.getPos()).normed();
        point.setNormal1(normal1);
        point.setNormal2(-normal1);

        point.setDepth(c1.getRadius() - v1.length());

        return true;
    } else if (projL > v2.length() &&
               (c1.getPos() - l2.getP2()).length() < c1.getRadius()) {
        point.setPos(l2.getP2());

        vec2d normal1 = (l2.getP2() - c1.getPos()).normed();
        point.setNormal1(normal1);
        point.setNormal2(-normal1);

        point.setDepth(c1.getRadius() - (l2.getP2() - c1.getPos()).length());

        return true;
    }

    return false;
}

bool collisionPrimitives(const circle2d &c1, const rectangle2d &r2,
                         collisionPrimitivesPoint &point) {
    bool result = collisionPrimitives(r2, c1, point);
    point.swap();
    return result;
}

bool collisionPrimitives(const line2d &l1, const circle2d &c2,
                         collisionPrimitivesPoint &point) {
    bool result = collisionPrimitives(c2, l1, point);
    point.swap();
    return result;
}

bool collisionPrimitives(const line2d &l1, const line2d &l2,
                         collisionPrimitivesPoint &point) {
    double run1 = l1.getP2().x() - l1.getP1().x();
    if (std::abs(run1) < std::numeric_limits<double>::epsilon())
        run1 = 0.1;
    double run2 = l2.getP2().x() - l2.getP1().x();
    if (std::abs(run2) < std::numeric_limits<double>::epsilon())
        run2 = 0.1;

    double rise1 = l1.getP2().y() - l1.getP1().y();
    if (std::abs(rise1) < std::numeric_limits<double>::epsilon())
        rise1 = 0.1;
    double a1 = rise1 / run1;

    double rise2 = l2.getP2().y() - l2.getP1().y();
    if (std::abs(rise2) < std::numeric_limits<double>::epsilon())
        rise2 = 0.1;
    double a2 = rise2 / run2;

    double b1 = -a1 * l1.getP2().x() + l1.getP2().y();
    double b2 = -a2 * l2.getP2().x() + l2.getP2().y();

    double x_intersect = (b2 - b1) / (a1 - a2);
    double y_intersect = a1 * x_intersect + b1;

    if (((x_intersect >= std::min(l1.getP1().x(), l1.getP2().x()) &&
          x_intersect <= std::max(l1.getP1().x(), l1.getP2().x())) ||
         (y_intersect >= std::min(l1.getP1().y(), l1.getP2().y()) &&
          y_intersect <= std::max(l1.getP1().y(), l1.getP2().y()))) &&

        ((x_intersect >= std::min(l2.getP1().x(), l2.getP2().x()) &&
          x_intersect <= std::max(l2.getP1().x(), l2.getP2().x())) ||
         (y_intersect >= std::min(l2.getP1().y(), l2.getP2().y()) &&
          y_intersect <= std::max(l2.getP1().y(), l2.getP2().y())))) {
        point.setPos(vec2d(x_intersect, y_intersect));
        point.setNormal1((l1.getP2() - l1.getP1()).perp().norm());
        point.setNormal2((l2.getP2() - l2.getP1()).perp().norm());

        double dp11 =
            point.getNormal1().dotProduct(l2.getP1() - point.getPos());
        double dp12 =
            point.getNormal1().dotProduct(l2.getP2() - point.getPos());

        double dp21 =
            point.getNormal2().dotProduct(l1.getP1() - point.getPos());
        double dp22 =
            point.getNormal2().dotProduct(l1.getP2() - point.getPos());

        point.setDepth(std::max(std::abs(std::min(dp21, dp22)),
                                std::abs(std::min(dp11, dp12))));

        return true;
    }

    return false;
}

bool collisionPrimitives(const line2d &l1, const rectangle2d &r2,
                         collisionPrimitivesPoint &point) {
    return false;
}

bool collisionPrimitives(const rectangle2d &r1, const circle2d &c2,
                         collisionPrimitivesPoint &point) {
    vec2d vX = r1.P2() - r1.P3();
    vec2d vY = r1.P4() - r1.P3();
    vec2d vC = c2.getPos() - r1.P3();
    double projX = vC.dotProduct(vX) / r1.getSize().x();
    double projY = vC.dotProduct(vY) / r1.getSize().y();

    if (projX >= 0 && projX <= r1.getSize().x()) {
        if (projY < r1.getSize().y() / 2 && projY + c2.getRadius() >= 0) {
            point.setPos(vX / r1.getSize().x() * projX + r1.P3());
            point.setNormal2(vY.normed());
            point.setNormal1(-point.getNormal2());
            point.setDepth(projY + c2.getRadius());
            return true;
        } else if (projY > r1.getSize().y() / 2 &&
                   (projY - c2.getRadius()) <= r1.getSize().y()) {
            point.setPos(vX / r1.getSize().x() * projX + r1.P4());
            point.setNormal1(vY.normed());
            point.setNormal2(-point.getNormal1());
            point.setDepth(r1.getSize().y() - (projY - c2.getRadius()));
            return true;
        }
    } else if (projY >= 0 && projY <= r1.getSize().y()) {
        if (projX < r1.getSize().x() / 2 && projX + c2.getRadius() >= 0) {
            point.setPos(vY / r1.getSize().y() * projY + r1.P3());
            point.setNormal2(vX.normed());
            point.setNormal1(-point.getNormal2());
            point.setDepth(projX + c2.getRadius());
            return true;
        } else if (projX > r1.getSize().x() / 2 &&
                   (projX - c2.getRadius()) <= r1.getSize().x()) {
            point.setPos(vY / r1.getSize().y() * projY + r1.P2());
            point.setNormal1(vX.normed());
            point.setNormal2(-point.getNormal1());
            point.setDepth(r1.getSize().x() - (projX - c2.getRadius()));
            return true;
        }
    } else if (projX < 0 && projY < 0) {
        vec2d v = c2.getPos() - r1.P3();
        double vL = v.length();
        if (vL <= c2.getRadius()) {
            point.setPos(r1.P3());
            point.setNormal1(v / vL);
            point.setNormal2(-point.getNormal1());
            point.setDepth(c2.getRadius() - vL);
            return true;
        }
    } else if (projX < 0 && projY > r1.getSize().y()) {
        vec2d v = c2.getPos() - r1.P4();
        double vL = v.length();
        if (vL <= c2.getRadius()) {
            point.setPos(r1.P4());
            point.setNormal1(v / vL);
            point.setNormal2(-point.getNormal1());
            point.setDepth(c2.getRadius() - vL);
            return true;
        }
    } else if (projX > r1.getSize().x() && projY > r1.getSize().y()) {
        vec2d v = c2.getPos() - r1.P1();
        double vL = v.length();
        if (vL <= c2.getRadius()) {
            point.setPos(r1.P1());
            point.setNormal1(v / vL);
            point.setNormal2(-point.getNormal1());
            point.setDepth(c2.getRadius() - vL);
            return true;
        }
    } else if (projX > r1.getSize().x() && projY < 0) {
        vec2d v = c2.getPos() - r1.P2();
        double vL = v.length();
        if (vL <= c2.getRadius()) {
            point.setPos(r1.P2());
            point.setNormal1(v / vL);
            point.setNormal2(-point.getNormal1());
            point.setDepth(c2.getRadius() - vL);
            return true;
        }
    }

    return false;
}

bool collisionPrimitives(const rectangle2d &r1, const line2d &l2,
                         collisionPrimitivesPoint &point) {
    return false;
}

bool collisionPrimitives(const rectangle2d &r1, const rectangle2d &r2,
                         collisionPrimitivesPoint &point) {
    vec2d vX = r1.P2() - r1.P3();
    vec2d vY = r1.P4() - r1.P3();

    vec2d v1(vX.dotProduct(r2.P1() - r1.P3()) / r1.getSize().x(),
             vY.dotProduct(r2.P1() - r1.P3()) / r1.getSize().y());
    vec2d v2(vX.dotProduct(r2.P2() - r1.P3()) / r1.getSize().x(),
             vY.dotProduct(r2.P2() - r1.P3()) / r1.getSize().y());
    vec2d v3(vX.dotProduct(r2.P3() - r1.P3()) / r1.getSize().x(),
             vY.dotProduct(r2.P3() - r1.P3()) / r1.getSize().y());
    vec2d v4(vX.dotProduct(r2.P4() - r1.P3()) / r1.getSize().x(),
             vY.dotProduct(r2.P4() - r1.P3()) / r1.getSize().y());

    return false;
}

bool collisionPrimitives(const primitive2d &p1, const primitive2d &p2,
                         collisionPrimitivesPoint &point) {
    if (typeid(p1) == typeid(circle2d)) {
        if (typeid(p2) == typeid(circle2d))
            return collisionPrimitives(static_cast<const circle2d &>(p1),
                                       static_cast<const circle2d &>(p2),
                                       point);
        else if (typeid(p2) == typeid(line2d))
            return collisionPrimitives(static_cast<const circle2d &>(p1),
                                       static_cast<const line2d &>(p2), point);
        else if (typeid(p2) == typeid(rectangle2d))
            return collisionPrimitives(static_cast<const circle2d &>(p1),
                                       static_cast<const rectangle2d &>(p2),
                                       point);
    } else if (typeid(p1) == typeid(line2d)) {
        if (typeid(p2) == typeid(circle2d))
            return collisionPrimitives(static_cast<const line2d &>(p1),
                                       static_cast<const circle2d &>(p2),
                                       point);
        else if (typeid(p2) == typeid(line2d))
            return collisionPrimitives(static_cast<const line2d &>(p1),
                                       static_cast<const line2d &>(p2), point);
        else if (typeid(p2) == typeid(rectangle2d))
            return collisionPrimitives(static_cast<const line2d &>(p1),
                                       static_cast<const rectangle2d &>(p2),
                                       point);
    } else if (typeid(p1) == typeid(rectangle2d)) {
        if (typeid(p2) == typeid(circle2d))
            return collisionPrimitives(static_cast<const rectangle2d &>(p1),
                                       static_cast<const circle2d &>(p2),
                                       point);
        else if (typeid(p2) == typeid(line2d))
            return collisionPrimitives(static_cast<const rectangle2d &>(p1),
                                       static_cast<const line2d &>(p2), point);
        else if (typeid(p2) == typeid(rectangle2d))
            return collisionPrimitives(static_cast<const rectangle2d &>(p1),
                                       static_cast<const rectangle2d &>(p2),
                                       point);
    }
    return false;
}