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
    doneCurrent();
}

void GLWidget::initializeGL() {
    initializeOpenGLFunctions();

    world.setCamera(camera2d{0, 0, 0, 60, 60});

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);
    for (int i = 0; i < 10; i++) {
        double pos_x = dis(gen) * 60 - 30;
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

    QOpenGLShader *vshader = new QOpenGLShader(QOpenGLShader::Vertex, this);
    QFile file_vshader(":/rcc/vertex_shader.glsl");
    if (file_vshader.open(QIODevice::ReadOnly)) {
        qint64 fileSize = file_vshader.size();
        QByteArray fileContent(fileSize, '\0');
        qint64 bytesRead = file_vshader.read(fileContent.data(), fileSize);
        vshader->compileSourceCode(fileContent.constData());
    } else
        qDebug() << "Unable to open: " << file_vshader.fileName();

    QOpenGLShader *fshader = new QOpenGLShader(QOpenGLShader::Fragment, this);
    QFile file_fshader(":/rcc/fragment_shader.glsl");
    if (file_fshader.open(QIODevice::ReadOnly)) {
        qint64 fileSize = file_fshader.size();
        QByteArray fileContent(fileSize, '\0');
        qint64 bytesRead = file_fshader.read(fileContent.data(), fileSize);
        fshader->compileSourceCode(fileContent.constData());
    } else
        qDebug() << "Unable to open: " << file_fshader.fileName();

    program = new QOpenGLShaderProgram;
    program->addShader(vshader);
    program->addShader(fshader);
    program->bindAttributeLocation("vertex", PROGRAM_VERTEX_ATTRIBUTE);
    program->bindAttributeLocation("texCoord", PROGRAM_TEXCOORD_ATTRIBUTE);
    program->link();

    program->bind();
    program->setUniformValue("texture", 0);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [this]() { update(); });
    timer->start(1000 / 100);
}

void GLWidget::updateMatrix() {
    m.setToIdentity();

    float x_coef = 1;
    float y_coef = 1;
    if (width() > height())
        x_coef = float(width()) / height();
    else
        y_coef = float(height()) / width();
    m.ortho(-world.getCamera().getCameraWidth() / 2 * x_coef,
            +world.getCamera().getCameraWidth() / 2 * x_coef,
            -world.getCamera().getCameraHeight() / 2 * y_coef,
            +world.getCamera().getCameraHeight() / 2 * y_coef, -15.0f, 30.0f);
}

void GLWidget::paintGL() {
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    updateMatrix();

    for (auto &mesh : world)
        mesh.setAngle(mesh.getAngle() + 0.01);
    world.precalc();
    for (auto &mesh : world) {
        mesh.getTexture()->bind();
        mesh.getVBO()->bind();
        // program->bind();

        program->setUniformValue("matrix", m);
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
        // program->release();
    }
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
        nearest_mesh->explosion(camera_point, world.getCamera());
        update();
    } else if (event->buttons() & Qt::RightButton) {
        auto v = event->pos() - oldPos;

        auto size = qMin(width(), height());
        nearest_mesh->setPosX(nearest_mesh->getPosX() +
                              v.rx() * world.getCamera().getCameraWidth() /
                                  size);
        nearest_mesh->setPosY(nearest_mesh->getPosY() -
                              v.ry() * world.getCamera().getCameraHeight() /
                                  size);

        oldPos = event->pos();
        update();
    }
}

void GLWidget::mouseReleaseEvent(QMouseEvent * /* event */) { emit clicked(); }
