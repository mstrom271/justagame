#pragma once

#include "camera2d.h"
#include "math2d.h"
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <vector>

class mesh2d {
    double pos_x = 0, pos_y = 0, angle = 0;
    int grid_size_x, grid_size_y;
    std::vector<vec2d> mesh_vec;

    QOpenGLTexture *texture = nullptr;
    QOpenGLBuffer *vbo = nullptr;

    bool cache_invalid = true;

  public:
    mesh2d(int grid_size_x, int grid_size_y, float width, float height,
           const QImage &qimage)
        : grid_size_x(grid_size_x), grid_size_y(grid_size_y),
          texture(new QOpenGLTexture(qimage)) {
        float step_x = width / (grid_size_x - 1);
        float step_y = height / (grid_size_y - 1);

        for (int x = 0; x < grid_size_x; x++)
            for (int y = 0; y < grid_size_y; y++) {
                mesh_vec.push_back(
                    vec2d(-width / 2 + x * step_x, -height / 2 + y * step_y));
            }

        vbo = new QOpenGLBuffer();
        vbo->create();
        vbo->bind();
        vbo->allocate(getOpenglVerticiesSize() * sizeof(GLfloat));
        vbo->release();
    }

    mesh2d(mesh2d &&mesh) {
        pos_x = mesh.pos_x;
        pos_y = mesh.pos_y;
        angle = mesh.angle;
        grid_size_x = mesh.grid_size_x;
        grid_size_y = mesh.grid_size_y;
        mesh_vec = std::move(mesh.mesh_vec);

        texture = mesh.texture;
        mesh.texture = nullptr;

        vbo = mesh.vbo;
        mesh.vbo = nullptr;
    }

    ~mesh2d() {
        delete vbo;
        delete texture;
    }

    void precalc(const camera2d &camera) {
        if (!cache_invalid)
            return;

        std::vector<float> openglVerticies;
        for (int x = 0; x < grid_size_x - 1; x++)
            for (int y = 0; y < grid_size_y - 1; y++) {
                vec2d v;
                v = mesh_vec[x * grid_size_y + y];
                openglVerticies.push_back(v.x());
                openglVerticies.push_back(v.y());

                openglVerticies.push_back(static_cast<float>(x) /
                                          (grid_size_x - 1));
                openglVerticies.push_back(static_cast<float>(y) /
                                          (grid_size_y - 1));

                v = mesh_vec[(x + 1) * grid_size_y + y];
                openglVerticies.push_back(v.x());
                openglVerticies.push_back(v.y());

                openglVerticies.push_back(static_cast<float>(x + 1) /
                                          (grid_size_x - 1));
                openglVerticies.push_back(static_cast<float>(y) /
                                          (grid_size_y - 1));

                v = mesh_vec[(x + 1) * grid_size_y + (y + 1)];
                openglVerticies.push_back(v.x());
                openglVerticies.push_back(v.y());

                openglVerticies.push_back(static_cast<float>(x + 1) /
                                          (grid_size_x - 1));
                openglVerticies.push_back(static_cast<float>(y + 1) /
                                          (grid_size_y - 1));

                v = mesh_vec[x * grid_size_y + y];
                openglVerticies.push_back(v.x());
                openglVerticies.push_back(v.y());

                openglVerticies.push_back(static_cast<float>(x) /
                                          (grid_size_x - 1));
                openglVerticies.push_back(static_cast<float>(y) /
                                          (grid_size_y - 1));

                v = mesh_vec[(x + 1) * grid_size_y + (y + 1)];
                openglVerticies.push_back(v.x());
                openglVerticies.push_back(v.y());

                openglVerticies.push_back(static_cast<float>(x + 1) /
                                          (grid_size_x - 1));
                openglVerticies.push_back(static_cast<float>(y + 1) /
                                          (grid_size_y - 1));

                v = mesh_vec[x * grid_size_y + (y + 1)];
                openglVerticies.push_back(v.x());
                openglVerticies.push_back(v.y());

                openglVerticies.push_back(static_cast<float>(x) /
                                          (grid_size_x - 1));
                openglVerticies.push_back(static_cast<float>(y + 1) /
                                          (grid_size_y - 1));
            }

        vbo->bind();
        vbo->write(0, openglVerticies.data(),
                   getOpenglVerticiesSize() * sizeof(GLfloat));
        vbo->release();
        cache_invalid = false;
    }

    QOpenGLBuffer *getVBO() { return vbo; }

    size_t getOpenglVerticiesSize() {
        return (grid_size_x - 1) * (grid_size_y - 1) * 24;
    }

    QOpenGLTexture *getTexture() { return texture; }

    void explosion(vec2d local_point) {
        for (auto &v : mesh_vec) {
            auto temp = v - local_point;
            v = v + temp.normed() * (std::exp(-0.5 * temp.length())) * 0.2;
        }
        cache_invalid = true;
    }

    void setPosX(double x) { pos_x = x; }
    void setPosY(double y) { pos_y = y; }
    void setAngle(double newAngle) { angle = newAngle; }
    double getPosX() const { return pos_x; }
    double getPosY() const { return pos_y; }
    double getAngle() const { return angle; }
};