#pragma once

#include "math2d.h"
#include "object2d.h"
#include "primitive2d.h"
#include <list>

struct Item {
    bBox bbox;
    object2d *object;
    primitive2d *primitive;
};

class KDTree2d {
    static constexpr std::size_t depthMax = 15;

    bBox bbox;
    std::vector<Item> list;

    KDTree2d *leaf1 = nullptr;
    KDTree2d *leaf2 = nullptr;

  public:
    KDTree2d(const bBox &bbox);
    ~KDTree2d();

    void addItem(const Item &item, std::size_t depth = 0);
    void precalcDebug_VBO(std::vector<float> &vertices);
    void parseTree(std::vector<std::pair<Item, Item>> &result);
    void getObject(const bBox &bbox);
};