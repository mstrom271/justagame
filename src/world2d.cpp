#include "world2d.h"

world2d::~world2d() {
    for (size_t i = 0; i < debug_VBO_number; i++) {
        delete debug_VBO_array[i];
        debug_VBO_array[i] = nullptr;
    }
}

void world2d::addObject(object2d *object) { objects.push_back(object); }
void world2d::deleteObject(object2d *object) { objects.remove(object); }
void world2d::addConnection(connection2d *connection) {
    connections.push_back(connection);
}
void world2d::deleteConnection(connection2d *connection) {
    connections.remove(connection);
}

std::list<object2d *> &world2d::getObjects() { return objects; }

void world2d::setCamera(const camera2d &newCamera) { camera = newCamera; }
camera2d world2d::getCamera() { return camera; }

void world2d::precalc(bool isDebug) {
    for (auto debug_VBO : debug_VBO_array) {
        delete debug_VBO;
        debug_VBO = nullptr;
    }

    std::vector<float> vertices[debug_VBO_number];

    bool collisionModelBBox_init = false;
    for (auto object : objects) {
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
    for (auto connection : connections)
        connection->precalcDebug_VBO(vertices[1]);

    delete kdtree;
    kdtree = new KDTree2d(collisionModel_bBox);
    for (auto object : objects)
        object->precalcCollisionModel_KDTree(kdtree);
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

void world2d::collisionDetection() {
    std::vector<std::pair<Item, Item>> result;
    kdtree->parseTree(result);
    std::sort(
        begin(result), end(result),
        [](const std::pair<Item, Item> &i, const std::pair<Item, Item> &j) {
            return i.first.object < j.first.object ||

                   i.first.object == j.first.object &&
                       i.second.object < j.second.object ||

                   i.first.object == j.first.object &&
                       i.second.object == j.second.object &&
                       i.first.primitive < j.first.primitive ||

                   i.first.object == j.first.object &&
                       i.second.object == j.second.object &&
                       i.first.primitive == j.first.primitive &&
                       i.second.primitive < j.second.primitive;
        });
    auto uniqueEnd = std::unique(
        begin(result), end(result),
        [](const std::pair<Item, Item> &i, const std::pair<Item, Item> &j) {
            return i.first.object == j.first.object &&
                   i.first.primitive == j.first.primitive &&
                   i.second.object == j.second.object &&
                   i.second.primitive == j.second.primitive;
        });
    result.erase(uniqueEnd, end(result));

    collisionPoints.clear();
    for (auto Items : result) {
        collisionPrimitivesPoint point;
        if (collisionPrimitives(*Items.first.primitive, *Items.second.primitive,
                                point)) {
            if (collisionPoints.empty() ||
                collisionPoints.back().getObj1() != Items.first.object ||
                collisionPoints.back().getObj2() != Items.second.object)

                collisionPoints.push_back(collisionObjectsPoint(
                    Items.first.object, Items.second.object, point.getPos(),
                    point.getNormal1(), point.getNormal2(), point.getDepth()));
            else {
                collisionPoints.back().setPos(
                    (collisionPoints.back().getPos() + point.getPos()) / 2);
                collisionPoints.back().setNormal1(
                    (collisionPoints.back().getNormal1() + point.getNormal1()) /
                    2);
                collisionPoints.back().setNormal2(
                    (collisionPoints.back().getNormal2() + point.getNormal2()) /
                    2);
                collisionPoints.back().setDepth(
                    (collisionPoints.back().getDepth() + point.getDepth()) / 2);
            }
        }
    }

    // qDebug() << result.size() << " " << collisionPoints.size();
}

void world2d::collisionResolve() {
    for (auto &point : collisionPoints) {
        // pull out objects from each other
        vec2d dist = point.getObj2()->getPos() - point.getObj1()->getPos();
        dist.norm();
        vec2d pullout_v1, pullout_v2;
        if (dist.dotProduct(point.getNormal1()) > 0.1)
            pullout_v1 = point.getNormal1();
        else
            pullout_v1 = dist;

        if (dist.dotProduct(point.getNormal2()) < 0.1)
            pullout_v2 = point.getNormal2();
        else
            pullout_v2 = -dist;

        if (!point.getObj1()->getIsFixed() && !point.getObj2()->getIsFixed()) {
            point.getObj1()->setPos(point.getObj1()->getPos() +
                                    pullout_v2 * point.getDepth() / 2);
            point.getObj2()->setPos(point.getObj2()->getPos() +
                                    pullout_v1 * point.getDepth() / 2);
        } else if (!point.getObj1()->getIsFixed()) {
            point.getObj1()->setPos(point.getObj1()->getPos() +
                                    pullout_v2 * point.getDepth());
        } else if (!point.getObj2()->getIsFixed()) {
            point.getObj2()->setPos(point.getObj2()->getPos() +
                                    pullout_v1 * point.getDepth());
        }
    }
}

void world2d::intersect(const primitive2d &primitive,
                        std::vector<Item> &result) {
    std::vector<Item> temp_result;
    kdtree->intersect(primitive.getBBox(), temp_result);

    collisionPrimitivesPoint p;
    for (const auto &item : temp_result)
        if (collisionPrimitives(*item.primitive, primitive, p))
            result.push_back(item);
}

void world2d::intersect(const vec2d &point, std::vector<Item> &result) {
    intersect(circle2d(point, 0.5), result);
    // TODO without using circle2d class
}

void world2d::update(double sec) {
    for (auto obj : objects) {
        obj->setPos(obj->getPos() + obj->getSpeed() * sec);
        obj->setAngle(obj->getAngle() + obj->getAngleSpeed() * sec);

        // slow down objects
        constexpr double viscosity = 0.005;
        obj->setSpeed(obj->getSpeed() * (1 - viscosity * sec));
        obj->setAngleSpeed(obj->getAngleSpeed() * (1 - viscosity * sec));
    }

    for (auto connection : connections) {
        vec2d worldPoint1 = connection->getWorldPoint1();
        vec2d worldPoint2 = connection->getWorldPoint2();

        if (connection->getObject1()) {
            vec2d dist = connection->getObject1()->worldToObject(worldPoint2) -
                         connection->getPoint1();
            connection->getObject1()->applyForceLocal(dist * 0.0002,
                                                      connection->getPoint1());
        }
        if (connection->getObject2()) {
            vec2d dist = connection->getObject2()->worldToObject(worldPoint1) -
                         connection->getPoint2();
            connection->getObject2()->applyForceLocal(dist * 0.0002,
                                                      connection->getPoint2());
        }
    }
}

QOpenGLBuffer *world2d::getDebug_VBO(std::size_t index) {
    return debug_VBO_array[index];
}
QVector4D world2d::getDebug_color(std::size_t index) {
    return debug_colors_array[index];
}

void world2d::destroy() { objects.clear(); }