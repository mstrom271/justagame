#pragma once

#include "math2d.h"
#include <vector>

class mesh2d {
    int grid_size_x, grid_size_y;
    std::vector<vec2d> mesh_vec;
    std::vector<float> openglVerticies_vec;

  public:
    mesh2d(int grid_size_x, int grid_size_y, float width, float height)
        : grid_size_x(grid_size_x), grid_size_y(grid_size_y) {

        float step_x = width / (grid_size_x - 1);
        float step_y = height / (grid_size_y - 1);

        for (int x = 0; x < grid_size_x; x++)
            for (int y = 0; y < grid_size_y; y++) {
                mesh_vec.push_back(
                    vec2d(-width / 2 + x * step_x, -height / 2 + y * step_y));
            }
    }

    const float *getOpenglVerticies() {
        openglVerticies_vec.clear();

        for (int x = 0; x < grid_size_x - 1; x++)
            for (int y = 0; y < grid_size_y - 1; y++) {
                vec2d v;
                v = mesh_vec[x * grid_size_y + y];
                openglVerticies_vec.push_back(v.x());
                openglVerticies_vec.push_back(v.y());

                openglVerticies_vec.push_back(static_cast<float>(x) /
                                              (grid_size_x - 1));
                openglVerticies_vec.push_back(static_cast<float>(y) /
                                              (grid_size_y - 1));

                v = mesh_vec[(x + 1) * grid_size_y + y];
                openglVerticies_vec.push_back(v.x());
                openglVerticies_vec.push_back(v.y());

                openglVerticies_vec.push_back(static_cast<float>(x + 1) /
                                              (grid_size_x - 1));
                openglVerticies_vec.push_back(static_cast<float>(y) /
                                              (grid_size_y - 1));

                v = mesh_vec[(x + 1) * grid_size_y + (y + 1)];
                openglVerticies_vec.push_back(v.x());
                openglVerticies_vec.push_back(v.y());

                openglVerticies_vec.push_back(static_cast<float>(x + 1) /
                                              (grid_size_x - 1));
                openglVerticies_vec.push_back(static_cast<float>(y + 1) /
                                              (grid_size_y - 1));

                v = mesh_vec[x * grid_size_y + y];
                openglVerticies_vec.push_back(v.x());
                openglVerticies_vec.push_back(v.y());

                openglVerticies_vec.push_back(static_cast<float>(x) /
                                              (grid_size_x - 1));
                openglVerticies_vec.push_back(static_cast<float>(y) /
                                              (grid_size_y - 1));

                v = mesh_vec[(x + 1) * grid_size_y + (y + 1)];
                openglVerticies_vec.push_back(v.x());
                openglVerticies_vec.push_back(v.y());

                openglVerticies_vec.push_back(static_cast<float>(x + 1) /
                                              (grid_size_x - 1));
                openglVerticies_vec.push_back(static_cast<float>(y + 1) /
                                              (grid_size_y - 1));

                v = mesh_vec[x * grid_size_y + (y + 1)];
                openglVerticies_vec.push_back(v.x());
                openglVerticies_vec.push_back(v.y());

                openglVerticies_vec.push_back(static_cast<float>(x) /
                                              (grid_size_x - 1));
                openglVerticies_vec.push_back(static_cast<float>(y + 1) /
                                              (grid_size_y - 1));
            }
        return openglVerticies_vec.data();
    }

    size_t getSize() { return (grid_size_x - 1) * (grid_size_y - 1) * 24; }

    void explosion(vec2d v) {
        for (auto &i : mesh_vec) {
            auto temp = i - v;
            i = i + temp.normed() * (std::exp(-0.5 * temp.length())) * 0.1;
        }
    }
};