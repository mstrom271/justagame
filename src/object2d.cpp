#include "object2d.h"
#include "kdtree2d.h"
#include "math2d.h"
#include <cmath>
#include <iterator>
#include <list>
#include <unordered_map>
#include <vector>

object2d::~object2d() {
    for (auto p : collisionModel)
        delete p;

    for (auto p : collisionModel_precalc)
        delete p;

    delete displayModel_texture;
    delete displayModel_VBO;
}

vec2d object2d::getPos() const { return pos; }
void object2d::setPos(const vec2d &newPos) {
    pos = newPos;
    collisionModel_expired = true;
}
double object2d::getAngle() const { return angle; }
void object2d::setAngle(double newAngle) {
    angle = newAngle;
    collisionModel_expired = true;
}

vec2d object2d::getSpeed() const { return speed; }
void object2d::setSpeed(vec2d newSpeed) { speed = newSpeed; }
double object2d::getAngleSpeed() const { return angleSpeed; }
void object2d::setAngleSpeed(double newAngleSpeed) {
    angleSpeed = newAngleSpeed;
}

void object2d::add(primitive2d *p) {
    collisionModel.push_back(p);

    collisionModel_expired = true;
    displayModel_VBO_expired = true;
}

void object2d::explosion(vec2d local_point) {
    auto transform = [&local_point](vec2d v) {
        auto temp = v - local_point;
        return v + temp.normed() * (std::exp(-0.5 * temp.length())) * 0.2;
    };

    for (auto p : collisionModel) {
        if (typeid(*p) == typeid(circle2d)) {
            circle2d *c = static_cast<circle2d *>(p);
            c->setPos(transform(c->getPos()));
        } else if (typeid(*p) == typeid(line2d)) {
            line2d *l = static_cast<line2d *>(p);
            l->setP1(transform(l->getP1()));
            l->setP2(transform(l->getP2()));
        } else if (typeid(*p) == typeid(rectangle2d)) {
            rectangle2d *r = static_cast<rectangle2d *>(p);
            r->setPos(transform(r->getPos()));
        }
    }

    collisionModel_expired = true;
    displayModel_VBO_expired = true;
}

void object2d::precalcCollisionModel() {
    if (!collisionModel_expired)
        return;

    mat23 matrix;
    matrix.rotate(angle);
    matrix.translate(pos);

    collisionModel_precalc.clear();
    for (auto p : collisionModel) {
        collisionModel_precalc.push_back(p->clone());
        collisionModel_precalc.back()->precalc(matrix);
    }

    bool collisionModelBBox_init = false;
    for (auto p : collisionModel_precalc) {
        if (collisionModelBBox_init)
            collisionModel_bBox += p->getBBox();
        else {
            collisionModel_bBox = p->getBBox();
            collisionModelBBox_init = true;
        }
    }

    collisionModel_expired = false;
}

void object2d::precalcCollisionModel_KDTree(KDTree2d *kdtree) {
    for (auto p : collisionModel_precalc)
        kdtree->addItem(Item{p->getBBox(), this, p});
}

QOpenGLTexture *object2d::getDisplayModel_texture() {
    return displayModel_texture;
}
QOpenGLBuffer *object2d::getDisplayModel_VBO() { return displayModel_VBO; }

void object2d::precalcDebug_VBO(std::vector<float> &vertices) {
    for (auto p : collisionModel_precalc) {
        if (typeid(*p) == typeid(circle2d))
            pushCircleVertices(vertices, static_cast<circle2d *>(p));
        else if (typeid(*p) == typeid(line2d))
            pushLineVertices(vertices, static_cast<line2d *>(p));
        else if (typeid(*p) == typeid(rectangle2d))
            pushRectangleVertices(vertices, static_cast<rectangle2d *>(p));
        // pushBBoxVertices(vertices, p->getBBox());
    }
    // pushBBoxVertices(vertices, collisionModel_bBox);
}

void object2d::precalcDisplayModel() {
    if (!displayModel_VBO_expired)
        return;

    delete displayModel_VBO;
    displayModel_VBO = nullptr;

    // TODO: precalcVBO for nonDebug

    displayModel_VBO_expired = false;
}

bBox object2d::getBBox() { return collisionModel_bBox; }

// bool object2d::collisionDetection(object2d &obj, collisionObjectsPoint
// &point) {
//     precalc();
//     obj.precalc();

//     if (!getBBox().intersect(obj.getBBox()))
//         return false;

//     std::list<collisionPrimitivesPoint> collisionPrimitivesPoints;
//     for (auto i = std::cbegin(precalcPrimitives);
//          i != std::cend(precalcPrimitives); i++) {
//         for (auto j = std::cbegin(obj.precalcPrimitives);
//              j != std::cend(obj.precalcPrimitives); j++) {
//             collisionPrimitivesPoint pointP;
//             if (i->collisionDetection(*j, pointP)) {
//                 collisionPrimitivesPoints.push_back(pointP);
//             }
//         }
//     }

//     if (!collisionPrimitivesPoints.empty()) {
//         vec2d _pos, normal1, normal2;
//         double depth = 0;
//         bool firstPass = true;
//         for (auto &point : collisionPrimitivesPoints) {
//             if (firstPass) {
//                 _pos = point.getPos();
//                 normal1 = point.getNormal1();
//                 normal2 = point.getNormal2();
//                 depth = point.getDepth();
//                 firstPass = false;
//             } else {
//                 _pos += point.getPos();
//                 normal1 += point.getNormal1();
//                 normal2 += point.getNormal2();
//                 depth = std::max(point.getDepth(), depth);
//             }
//         }
//         _pos /= collisionPrimitivesPoints.size();
//         //        normal1 /= collisionPrimitivesPoints.size();
//         //        normal2 /= collisionPrimitivesPoints.size();
//         normal1.norm();
//         normal2.norm();

//         point =
//             collisionObjectsPoint(this, &obj, _pos, normal1, normal2, depth);
//         return true;
//     }

//     return false;
// }

// // fabrics
// void makeCircle2d(object2d *obj, const vec2d &pos, double radius) {
//     obj->add(new circle2d(pos, radius));
// }

// void makeLine2d(object2d *obj, const vec2d &p1, const vec2d &p2) {
//     obj->add(new line2d(p1, p2));
// }

// void makeRectangle2d(object2d *obj, const vec2d &pos, const vec2d &size,
//                      double angle) {
//     //    vec2d a = vec2d(pos.x() - size.x(), pos.y() - size.y());
//     //    vec2d b = vec2d(pos.x() - size.x(), pos.y() + size.y());
//     //    vec2d c = vec2d(pos.x() + size.x(), pos.y() + size.y());
//     //    vec2d d = vec2d(pos.x() + size.x(), pos.y() - size.y());

//     //    a.rotate(angle);
//     //    b.rotate(angle);
//     //    c.rotate(angle);
//     //    d.rotate(angle);

//     //    obj->add(new line2d(a, b));
//     //    obj->add(new line2d(b, c));
//     //    obj->add(new line2d(c, d));
//     //    obj->add(new line2d(d, a));
//     obj->add(new rectangle2d(pos, size, angle));
// }

// collisionObjectsPoint::collisionObjectsPoint(object2d *obj1, object2d *obj2,
//                                              const vec2d &pos,
//                                              const vec2d &normal1,
//                                              const vec2d &normal2, double
//                                              depth)
//     : obj1(obj1), obj2(obj2), pos(pos), normal1(normal1), normal2(normal2),
//       depth(depth) {}
// object2d *collisionObjectsPoint::getObj1() const { return obj1; }
// void collisionObjectsPoint::setObj1(object2d *newObj1) { obj1 = newObj1; }
// object2d *collisionObjectsPoint::getObj2() const { return obj2; }
// void collisionObjectsPoint::setObj2(object2d *newObj2) { obj2 = newObj2; }
// vec2d collisionObjectsPoint::getPos() const { return pos; }
// void collisionObjectsPoint::setPos(const vec2d &newPos) { pos = newPos; }
// vec2d collisionObjectsPoint::getNormal1() const { return normal1; }
// void collisionObjectsPoint::setNormal1(const vec2d &newNormal1) {
//     normal1 = newNormal1;
// }
// vec2d collisionObjectsPoint::getNormal2() const { return normal2; }
// void collisionObjectsPoint::setNormal2(const vec2d &newNormal2) {
//     normal2 = newNormal2;
// }
// double collisionObjectsPoint::getDepth() const { return depth; }
// void collisionObjectsPoint::setDepth(double newDepth) { depth = newDepth; }

// void collisionObjectsPoint::draw(QPainter &painter) const {
//     painter.drawEllipse(pos, 20, 20);
//     painter.drawLine(obj1->getPos(), obj1->getPos() + normal1 * 20);
//     painter.drawLine(obj2->getPos(), obj2->getPos() + normal2 * 20);
// }
