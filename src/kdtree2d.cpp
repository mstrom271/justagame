#include "kdtree2d.h"
#include <QDebug>

float bBoxIntersectCoef(const bBox &bigBBox, const bBox &smallBBox) {
    float minX = std::min(bigBBox.getMinX(), smallBBox.getMinX());
    float maxX = std::max(bigBBox.getMaxX(), smallBBox.getMaxX());
    float diffX = bigBBox.width() + smallBBox.width() - (maxX - minX);

    float minY = std::min(bigBBox.getMinY(), smallBBox.getMinY());
    float maxY = std::max(bigBBox.getMaxY(), smallBBox.getMaxY());
    float diffY = bigBBox.height() + smallBBox.height() - (maxY - minY);

    return std::max(bigBBox.width() / diffX, bigBBox.height() / diffY);
}

enum class SplitType { Leaf1, Leaf2 };
bBox splitLeaf(const bBox &bbox, std::size_t depth, SplitType type) {
    if (type == SplitType::Leaf1) {
        if (depth % 2 == 0)
            // vertical
            return bBox(bbox.getMinX(), bbox.getMinY(),
                        bbox.getMaxX() - bbox.width() / 2, bbox.getMaxY());
        else
            // horizontal
            return bBox(bbox.getMinX(), bbox.getMinY(), bbox.getMaxX(),
                        bbox.getMaxY() - bbox.height() / 2);
    } else {
        if (depth % 2 == 0)
            // vertical
            return bBox(bbox.getMinX() + bbox.width() / 2, bbox.getMinY(),
                        bbox.getMaxX(), bbox.getMaxY());
        else
            // horizontal
            return bBox(bbox.getMinX(), bbox.getMinY() + bbox.height() / 2,
                        bbox.getMaxX(), bbox.getMaxY());
    }
}

KDTree2d::KDTree2d(const bBox &bbox) : bbox(bbox) {}
KDTree2d::~KDTree2d() {
    delete leaf1;
    delete leaf2;
}

void KDTree2d::addItem(const Item &item, std::size_t depth) {
    if (bBoxIntersectCoef(bbox, item.bbox) > bboxCoefMin && depth < depthMax) {
        if (leaf1 == nullptr)
            leaf1 = new KDTree2d(splitLeaf(bbox, depth, SplitType::Leaf1));
        if (leaf2 == nullptr)
            leaf2 = new KDTree2d(splitLeaf(bbox, depth, SplitType::Leaf2));

        if (item.bbox.intersect(leaf1->bbox))
            leaf1->addItem(item, depth + 1);
        if (item.bbox.intersect(leaf2->bbox))
            leaf2->addItem(item, depth + 1);
    } else
        list.push_back(item);
}

void KDTree2d::precalcDebug_VBO(std::vector<float> &vertices) {
    pushBBoxVertices(vertices, bbox);
    if (leaf1)
        leaf1->precalcDebug_VBO(vertices);
    if (leaf2)
        leaf2->precalcDebug_VBO(vertices);
}
