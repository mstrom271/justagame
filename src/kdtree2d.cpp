#include "kdtree2d.h"
#include <QDebug>

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
    list.push_back(item);

    if (((list.size() > 1 &&
          list[list.size() - 1].object != list[list.size() - 2].object) ||
         (leaf1 != nullptr && leaf2 != nullptr)) &&
        depth < depthMax) {

        while (!list.empty()) {
            if (leaf1 == nullptr)
                leaf1 = new KDTree2d(splitLeaf(bbox, depth, SplitType::Leaf1));
            if (leaf2 == nullptr)
                leaf2 = new KDTree2d(splitLeaf(bbox, depth, SplitType::Leaf2));

            if (list.back().bbox.intersect(leaf1->bbox))
                leaf1->addItem(list.back(), depth + 1);
            if (list.back().bbox.intersect(leaf2->bbox))
                leaf2->addItem(list.back(), depth + 1);
            list.pop_back();
        }
    }
}

void KDTree2d::precalcDebug_VBO(std::vector<float> &vertices) {
    pushBBoxVertices(vertices, bbox);
    if (leaf1)
        leaf1->precalcDebug_VBO(vertices);
    if (leaf2)
        leaf2->precalcDebug_VBO(vertices);
}

void KDTree2d::parseTree(std::vector<std::pair<Item, Item>> &result) {
    for (std::size_t i = 0; i < list.size(); i++)
        for (std::size_t j = i + 1; j < list.size(); j++)
            if (list[i].object < list[j].object)
                result.push_back({list[i], list[j]});
            else if (list[i].object > list[j].object)
                result.push_back({list[j], list[i]});

    if (leaf1)
        leaf1->parseTree(result);
    if (leaf2)
        leaf2->parseTree(result);
}

void KDTree2d::collisionDetection() {
    std::vector<std::pair<Item, Item>> result;
    parseTree(result);
    std::sort(
        begin(result), end(result),
        [](const std::pair<Item, Item> &i, const std::pair<Item, Item> &j) {
            return i.first.object < j.first.object ||

                   i.first.object == j.first.object &&
                       i.first.primitive < j.first.primitive ||

                   i.first.object == j.first.object &&
                       i.first.primitive == j.first.primitive &&
                       i.second.object < j.second.object ||

                   i.first.object == j.first.object &&
                       i.first.primitive == j.first.primitive &&
                       i.second.object == j.second.object &&
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

    // TODO

    qDebug() << result.size();
}
