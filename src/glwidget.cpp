#include "glwidget.h"
#include "math2d.h"
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

    world.setCamera(camera2d{0, 0, 0, 100, 100});

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);
    float coef = float(width()) / height();
    for (int i = 0; i < 1; i++) {
        double pos_x = (dis(gen) * 60 - 30) * coef;
        double pos_y = dis(gen) * 60 - 30;
        double angle = dis(gen) * 2 * pi;

        double speedX = dis(gen) * 0.1 - 0.05;
        double speedY = dis(gen) * 0.1 - 0.05;
        double speedAngle = dis(gen) * 0.001 - 0.0005;

        double width = 5 + dis(gen) * 20;
        double height = 5 + dis(gen) * 20;
        int grid_size_x = width * 0.5;
        int grid_size_y = height * 0.5;

        QImage image(":/rcc/brick.png");

        object2d *object = new object2d();
        for (int i = 0; i < grid_size_x; i++)
            for (int j = 0; j < grid_size_y; j++) {
                object->add(new circle2d(
                    {-width / 2 + i * (width / grid_size_x),
                     -height / 2 + j * (height / grid_size_y)},
                    std::min(width / grid_size_x, height / grid_size_y) / 2));
            }
        object->setPos({pos_x, pos_y});
        object->setAngle(angle);
        object->setSpeed({speedX, speedY});
        object->setAngleSpeed(speedAngle);
        // object->add(new line2d({-width / 2, -height / 1.5},
        //                        {width / 2, -height / 1.5}));
        // object->add(new rectangle2d({0, height / 2}, {width, height / 10},
        // 0));
        world.addObject(object);
    }

    object2d *object;
    for (int i = 0; i < 4; i++) {
        object = new object2d();
        object->add(new rectangle2d({0, 0}, {5, 90}, pi / 2 * i));
        object->setPos({50 * std::cos(pi / 2 * i), 50 * std::sin(pi / 2 * i)});
        object->setIsFixed(true);
        world.addObject(object);
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
    program_debug->bindAttributeLocation("vertex",
                                         PROGRAM_DEBUG_VERTEX_ATTRIBUTE);
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

    world_matrix.setToIdentity();
    world_matrix.ortho(-world.getCamera().getCameraWidth() / 2 * x_coef,
                       +world.getCamera().getCameraWidth() / 2 * x_coef,
                       -world.getCamera().getCameraHeight() / 2 * y_coef,
                       +world.getCamera().getCameraHeight() / 2 * y_coef,
                       -15.0f, 30.0f);
    world_matrix.rotate(-world.getCamera().getAngle() / (2 * pi) * 360, 0, 0,
                        1);
    world_matrix.translate(-world.getCamera().getPosX(),
                           -world.getCamera().getPosY());
}

void GLWidget::paintGL() {
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    updateMatrix();

    world.update(1000.0 / std::max(elapsedTimer->elapsed(), (long long)100));
    elapsedTimer->restart();

    bool isDebug = true;
    world.precalc(isDebug);
    world.collisionDetection();
    world.collisionResolve();
    // for (auto object : world) {
    //     QMatrix4x4 model_matrix = matrix;
    //     model_matrix.translate(object->getPos().x(), object->getPos().y());
    //     model_matrix.rotate(object->getAngle() / (2 * pi) * 360, 0, 0, 1);
    // }

    // Draw frame only for debug
    if (isDebug) {
        program_debug->bind();

        for (size_t i = 0; i < debug_VBO_number; i++) {
            world.getDebug_VBO(i)->bind();
            program_debug->setUniformValue("externalColor",
                                           world.getDebug_color(i));
            program_debug->setUniformValue("matrix", world_matrix);
            program_debug->enableAttributeArray(PROGRAM_DEBUG_VERTEX_ATTRIBUTE);
            program_debug->setAttributeBuffer(PROGRAM_DEBUG_VERTEX_ATTRIBUTE,
                                              GL_FLOAT, 0, 2,
                                              2 * sizeof(GLfloat));
            glDrawArrays(GL_LINES, 0, world.getDebug_VBO(i)->size());
            world.getDebug_VBO(i)->release();
        }
        program_debug->release();
    }
}

void GLWidget::resizeGL(int width, int height) {}

void GLWidget::keyPressEvent(QKeyEvent *event) {
    // if (event->key() == Qt::Key_Right) {
    // } else if (event->key() == Qt::Key_Left) {
    // }

    // QWidget::keyPressEvent(event);
}

void GLWidget::mousePressEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::RightButton) {
        grabbedRM = true;
    } else if (event->buttons() & Qt::LeftButton) {
        grabbedLM = true;

        vec2d worldPos =
            world.getCamera().cameraToWorld(widgetToCamera(event->pos()));
        vec2d localPos = (*world.getObjects().begin())->worldToObject(worldPos);

        mouse_connection = new connection2d(*world.getObjects().begin(),
                                            nullptr, localPos, worldPos);
        world.addConnection(mouse_connection);
    }

    // GLWidget::mousePressEvent(event);
}

void GLWidget::mouseMoveEvent(QMouseEvent *event) {
    vec2d worldPos =
        world.getCamera().cameraToWorld(widgetToCamera(event->pos()));
    vec2d localPos = (*world.getObjects().begin())->worldToObject(worldPos);
    if (grabbedLM)
        mouse_connection->setPoint2(worldPos);
    if (grabbedRM)
        (*world.getObjects().begin())->explosion(localPos);

    // auto size = qMin(width(), height());
    // vec2d camera_point((world.getCamera().getCameraWidth() / size) *
    //                        (-width() / 2 + event->pos().rx()),
    //                    (world.getCamera().getCameraHeight() / size) *
    //                        (height() / 2 - event->pos().ry()));

    // vec2d world_point = camera_point;
    // world_point.rotate(world.getCamera().getAngle());
    // world_point.translate(world.getCamera().getPosX(),
    //                       world.getCamera().getPosY());

    // object2d *nearest_object = nullptr;
    // double length = 0;
    // for (auto object : world.getObjects()) {
    //     if (!object->getIsFixed()) {
    //         double new_length = (object->getPos() - world_point).length();
    //         if (nearest_object == nullptr || new_length < length) {
    //             nearest_object = object;
    //             length = new_length;
    //         }
    //     }
    // }

    // if (event->buttons() & Qt::RightButton) {
    //     vec2d local_point = camera_point;
    //     local_point.rotate(world.getCamera().getAngle());
    //     local_point.translate(world.getCamera().getPosX(),
    //                           world.getCamera().getPosY());

    //     local_point.translate(-nearest_object->getPos().x(),
    //                           -nearest_object->getPos().y());
    //     local_point.rotate(-nearest_object->getAngle());

    //     nearest_object->explosion(local_point);
    //     update();
    // } else if (event->buttons() & Qt::LeftButton) {
    //     vec2d local_point = world_point;

    //     local_point.translate(-nearest_object->getPos().x(),
    //                           -nearest_object->getPos().y());
    //     local_point.rotate(-nearest_object->getAngle());

    //     mouse_connection->setObject1(nearest_object);
    //     mouse_connection->setPoint1(local_point);
    //     mouse_connection->setPoint2(world_point);

    //     grabbedLM = true;

    //     update();
    // }
    // GLWidget::mouseMoveEvent(event);
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (grabbedRM)
        grabbedRM = false;

    if (grabbedLM) {
        grabbedLM = false;
        world.deleteConnection(mouse_connection);
    }

    // GLWidget::mouseReleaseEvent(event);
}

vec2d GLWidget::widgetToCamera(QPoint pos) {
    auto size = qMin(width(), height());
    return {(world.getCamera().getCameraWidth() / size) *
                (-width() / 2 + pos.rx()),
            (world.getCamera().getCameraHeight() / size) *
                (height() / 2 - pos.ry())};
}

QPoint GLWidget::cameraToWidget(vec2d point) {
    // TODO
    return QPoint();
}
