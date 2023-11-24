#pragma once

#include "mesh2d.h"
#include <QMatrix4x4>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLWidget>

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram);
QT_FORWARD_DECLARE_CLASS(QOpenGLTexture)

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

  public:
    GLWidget(QWidget *widget = nullptr);
    ~GLWidget();

    void rotateBy(int xAngle, int yAngle, int zAngle);
    void setClearColor(const QColor &color);
    void updateMatrix();

  signals:
    void clicked();

  protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

  private:
    void makeObject();

    QColor clearColor = Qt::black;
    QPoint lastPos;
    int xRot = 0;
    int yRot = 0;
    int zRot = 0;
    QMatrix4x4 m;
    QOpenGLTexture *texture = nullptr;
    QOpenGLShaderProgram *program = nullptr;
    QOpenGLBuffer vbo;
    mesh2d mesh;

    static constexpr int PROGRAM_VERTEX_ATTRIBUTE = 0;
    static constexpr int PROGRAM_TEXCOORD_ATTRIBUTE = 1;
};