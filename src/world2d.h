#pragma once

#include "camera2d.h"
#include "mesh2d.h"
#include <list>

class world2d {
    camera2d camera;
    std::list<mesh2d> meshes;

  public:
    world2d() = default;

    void addMesh(mesh2d &&mesh) { meshes.push_back(std::move(mesh)); }

    auto begin() { return meshes.begin(); }
    auto end() { return meshes.end(); }

    void setCamera(const camera2d &newCamera) { camera = newCamera; }
    camera2d getCamera() { return camera; }

    void precalc() {
        for (auto &mesh : meshes)
            mesh.precalc(camera);
    }

    void destroy() { meshes.clear(); }
};
