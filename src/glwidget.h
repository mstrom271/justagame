#pragma once

#include "mesh2d.h"
#include "world2d.h"
#include <QElapsedTimer>
#include <QMatrix4x4>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <QTimer>

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram);
QT_FORWARD_DECLARE_CLASS(QOpenGLTexture)

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

  public:
    GLWidget(QWidget *widget = nullptr);
    ~GLWidget();

    void updateMatrix();

  signals:
    void clicked();

  protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

  private:
    world2d world;
    QMatrix4x4 world_matrix;
    QOpenGLShaderProgram *program = nullptr, *program_debug = nullptr;
    QPoint oldPos;
    QTimer *timer;
    QElapsedTimer *elapsedTimer;
    static constexpr int PROGRAM_VERTEX_ATTRIBUTE = 0;
    static constexpr int PROGRAM_TEXCOORD_ATTRIBUTE = 1;
    static constexpr int PROGRAM_DEBUG_VERTEX_ATTRIBUTE = 2;

    QOpenGLShader *load_shader(QString filename,
                               QOpenGLShader::ShaderType type);
};