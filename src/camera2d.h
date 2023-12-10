#pragma once

#include "math2d.h"

class camera2d {
    double pos_x, pos_y;
    double angle;
    double camera_width;
    double camera_height;

  public:
    camera2d() = default;
    camera2d(double pos_x, double pos_y, double angle, double camera_width,
             double camera_height)
        : pos_x(pos_x), pos_y(pos_y), angle(angle), camera_width(camera_width),
          camera_height(camera_height) {}

    void setPosX(double x) { pos_x = x; }
    void setPosY(double y) { pos_y = y; }
    void setAngle(double newAngle) { angle = newAngle; }
    void setCameraWidth(double width) { camera_width = width; }
    void setCameraHeight(double height) { camera_height = height; }
    double getPosX() const { return pos_x; }
    double getPosY() const { return pos_y; }
    double getAngle() const { return angle; }
    double getCameraWidth() const { return camera_width; }
    double getCameraHeight() const { return camera_height; }

    vec2d cameraToWorld(vec2d cameraPoint) {
        cameraPoint.rotate(getAngle());
        cameraPoint.translate(getPosX(), getPosY());
        return cameraPoint;
    }
    vec2d worldToCamera(vec2d worldPoint) {
        worldPoint.translate(-getPosX(), -getPosY());
        worldPoint.rotate(-getAngle());
        return worldPoint;
    }
};