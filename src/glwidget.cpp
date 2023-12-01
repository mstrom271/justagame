#include "glwidget.h"
#include <QColorTransform>
#include <QFile>
#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <cmath>
#include <ctime>
#include <random>

GLWidget::GLWidget(QWidget *widget){};

GLWidget::~GLWidget() {
    makeCurrent();
    world.destroy();
    delete program;
    delete program_debug;
    doneCurrent();
}

QOpenGLShader *GLWidget::load_shader(QString filename,
                                     QOpenGLShader::ShaderType type) {
    QOpenGLShader *shader = new QOpenGLShader(type, this);
    QFile file_shader(filename);
    if (file_shader.open(QIODevice::ReadOnly)) {
        qint64 fileSize = file_shader.size();
        QByteArray fileContent(fileSize, '\0');
        qint64 bytesRead = file_shader.read(fileContent.data(), fileSize);
        shader->compileSourceCode(fileContent.constData());
    } else
        qDebug() << "Unable to open: " << file_shader.fileName();

    return shader;
}

void GLWidget::initializeGL() {
    initializeOpenGLFunctions();

    world.setCamera(camera2d{0, 0, 0, 60, 60});

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);
    float coef = float(width()) / height();
    for (int i = 0; i < 10; i++) {
        double pos_x = (dis(gen) * 60 - 30) * coef;
        double pos_y = dis(gen) * 60 - 30;
        double angle = dis(gen) * 2 * M_PI;

        double width = 5 + dis(gen) * 20;
        double height = 5 + dis(gen) * 20;
        int grid_size_x = width * 4;
        int grid_size_y = height * 4;

        QImage image(":/rcc/brick.png");

        mesh2d mesh(grid_size_x, grid_size_y, width, height, image.mirrored());
        mesh.setPosX(pos_x);
        mesh.setPosY(pos_y);
        mesh.setAngle(angle);

        world.addMesh(std::move(mesh));
    }

    program = new QOpenGLShaderProgram;
    program->addShader(
        load_shader(":/rcc/vertex_shader.glsl", QOpenGLShader::Vertex));
    program->addShader(
        load_shader(":/rcc/fragment_shader.glsl", QOpenGLShader::Fragment));
    program->bindAttributeLocation("vertex", PROGRAM_VERTEX_ATTRIBUTE);
    program->bindAttributeLocation("texCoord", PROGRAM_TEXCOORD_ATTRIBUTE);
    program->link();
    program->bind();
    program->setUniformValue("texture", 0);
    program->release();

    program_debug = new QOpenGLShaderProgram;
    program_debug->addShader(
        load_shader(":/rcc/vertex_shader_debug.glsl", QOpenGLShader::Vertex));
    program_debug->addShader(load_shader(":/rcc/fragment_shader_debug.glsl",
                                         QOpenGLShader::Fragment));
    program_debug->bindAttributeLocation("vertex", PROGRAM_VERTEX_ATTRIBUTE);
    program_debug->link();

    elapsedTimer = new QElapsedTimer();
    elapsedTimer->start();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [this]() { update(); });
    timer->start(1000 / 100);
}

void GLWidget::updateMatrix() {
    float x_coef = 1;
    float y_coef = 1;
    if (width() > height())
        x_coef = float(width()) / height();
    else
        y_coef = float(height()) / width();

    matrix.setToIdentity();
    matrix.ortho(-world.getCamera().getCameraWidth() / 2 * x_coef,
                 +world.getCamera().getCameraWidth() / 2 * x_coef,
                 -world.getCamera().getCameraHeight() / 2 * y_coef,
                 +world.getCamera().getCameraHeight() / 2 * y_coef, -15.0f,
                 30.0f);
    matrix.rotate(-world.getCamera().getAngle() / (2 * M_PI) * 360, 0, 0, 1);
    matrix.translate(-world.getCamera().getPosX(),
                     -world.getCamera().getPosY());
}

void GLWidget::paintGL() {
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    for (auto &mesh : world)
        mesh.setAngle(mesh.getAngle() + 0.01);

    updateMatrix();
    world.precalc();
    world.precalcDebug();
    for (auto &mesh : world) {
        QMatrix4x4 model_matrix = matrix;
        model_matrix.translate(mesh.getPosX(), mesh.getPosY());
        model_matrix.rotate(mesh.getAngle() / (2 * M_PI) * 360, 0, 0, 1);

        // Draw triangles with texture
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        program->bind();
        mesh.getTexture()->bind();
        mesh.getVBO()->bind();
        program->setUniformValue("matrix", model_matrix);
        program->enableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
        program->enableAttributeArray(PROGRAM_TEXCOORD_ATTRIBUTE);
        program->setAttributeBuffer(PROGRAM_VERTEX_ATTRIBUTE, GL_FLOAT, 0, 2,
                                    4 * sizeof(GLfloat));
        program->setAttributeBuffer(PROGRAM_TEXCOORD_ATTRIBUTE, GL_FLOAT,
                                    2 * sizeof(GLfloat), 2,
                                    4 * sizeof(GLfloat));
        glDrawArrays(GL_TRIANGLES, 0, mesh.getOpenglVerticiesSize());
        mesh.getVBO()->release();
        mesh.getTexture()->release();
        program->release();

        // Draw frame only for debug
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        program_debug->bind();
        mesh.getVBODebug()->bind();
        program_debug->setUniformValue("externalColor", QVector4D(0, 1, 0, 1));
        program_debug->setUniformValue("matrix", model_matrix);
        program_debug->enableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
        program_debug->setAttributeBuffer(PROGRAM_VERTEX_ATTRIBUTE, GL_FLOAT, 0,
                                          2, 2 * sizeof(GLfloat));
        glDrawArrays(GL_LINES, 0, mesh.getOpenglVerticiesDebugSize());
        mesh.getVBODebug()->release();
        program_debug->release();
    }
    qDebug() << elapsedTimer->elapsed();
    elapsedTimer->restart();
}

void GLWidget::resizeGL(int width, int height) {}

void GLWidget::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Right) {
        world.begin()->setAngle(world.begin()->getAngle() - 0.1);
        update();
    } else if (event->key() == Qt::Key_Left) {
        world.begin()->setAngle(world.begin()->getAngle() + 0.1);
        update();
    }

    QWidget::keyPressEvent(event);
}

void GLWidget::mousePressEvent(QMouseEvent *event) { oldPos = event->pos(); }

void GLWidget::mouseMoveEvent(QMouseEvent *event) {
    auto size = qMin(width(), height());
    vec2d camera_point((world.getCamera().getCameraWidth() / size) *
                           (-width() / 2 + event->pos().rx()),
                       (world.getCamera().getCameraHeight() / size) *
                           (height() / 2 - event->pos().ry()));

    vec2d world_point = camera_point;
    world_point.rotate(world.getCamera().getAngle());
    world_point.translate(world.getCamera().getPosX(),
                          world.getCamera().getPosY());

    mesh2d *nearest_mesh = nullptr;
    double length = 0;
    for (auto &mesh : world) {
        double new_length =
            (vec2d(mesh.getPosX(), mesh.getPosY()) - world_point).length();
        if (nearest_mesh == nullptr || new_length < length) {
            nearest_mesh = &mesh;
            length = new_length;
        }
    }

    if (event->buttons() & Qt::LeftButton) {
        vec2d local_point = camera_point;
        local_point.rotate(world.getCamera().getAngle());
        local_point.translate(world.getCamera().getPosX(),
                              world.getCamera().getPosY());

        local_point.translate(-nearest_mesh->getPosX(),
                              -nearest_mesh->getPosY());
        local_point.rotate(-nearest_mesh->getAngle());

        nearest_mesh->explosion(local_point);
        update();
    } else if (event->buttons() & Qt::RightButton) {
        auto camera_v = event->pos() - oldPos;
        vec2d world_v = vec2d(camera_v);
        world_v.rotate(-world.getCamera().getAngle());

        auto size = qMin(width(), height());
        nearest_mesh->setPosX(nearest_mesh->getPosX() +
                              world_v.x() * world.getCamera().getCameraWidth() /
                                  size);
        nearest_mesh->setPosY(nearest_mesh->getPosY() -
                              world_v.y() *
                                  world.getCamera().getCameraHeight() / size);

        oldPos = event->pos();
        update();
    }
}

void GLWidget::mouseReleaseEvent(QMouseEvent * /* event */) { emit clicked(); }
