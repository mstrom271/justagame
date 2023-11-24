#include "glwidget.h"
#include <QFile>
#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>

GLWidget::GLWidget(QWidget *widget) : mesh(50, 50, 15, 15){};

GLWidget::~GLWidget() {
    makeCurrent();
    vbo.destroy();
    delete texture;
    delete program;
    doneCurrent();
}

void GLWidget::rotateBy(int xAngle, int yAngle, int zAngle) {
    xRot += xAngle;
    yRot += yAngle;
    zRot += zAngle;
    update();
}

void GLWidget::setClearColor(const QColor &color) {
    clearColor = color;
    update();
}

void GLWidget::initializeGL() {
    initializeOpenGLFunctions();

    texture = new QOpenGLTexture(QImage(":/rcc/brick.png").mirrored());
    vbo.create();
    vbo.bind();
    vbo.allocate(mesh.getOpenglVerticies(), mesh.getSize() * sizeof(GLfloat));

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
}

void GLWidget::updateMatrix() {
    m.setToIdentity();

    float x_coef = 1;
    float y_coef = 1;
    if (width() > height())
        x_coef = float(width()) / height();
    else
        y_coef = float(height()) / width();
    m.ortho(-10.0f * x_coef, +10.0f * x_coef, -10.0f * y_coef, +10.0f * y_coef,
            -15.0f, 30.0f);

    m.translate(0.0f, 0.0f, 0.0f);
    m.rotate(xRot / 16.0f, 1.0f, 0.0f, 0.0f);
    m.rotate(yRot / 16.0f, 0.0f, 1.0f, 0.0f);
    m.rotate(zRot / 16.0f, 0.0f, 0.0f, 1.0f);
}

void GLWidget::paintGL() {
    glClearColor(clearColor.redF(), clearColor.greenF(), clearColor.blueF(),
                 clearColor.alphaF());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    updateMatrix();

    vbo.bind();
    vbo.write(0, mesh.getOpenglVerticies(), mesh.getSize() * sizeof(GLfloat));
    program->bind();
    program->setUniformValue("matrix", m);
    program->enableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
    program->enableAttributeArray(PROGRAM_TEXCOORD_ATTRIBUTE);
    program->setAttributeBuffer(PROGRAM_VERTEX_ATTRIBUTE, GL_FLOAT, 0, 2,
                                4 * sizeof(GLfloat));
    program->setAttributeBuffer(PROGRAM_TEXCOORD_ATTRIBUTE, GL_FLOAT,
                                2 * sizeof(GLfloat), 2, 4 * sizeof(GLfloat));

    texture->bind();
    glDrawArrays(GL_TRIANGLES, 0, mesh.getSize());
}

void GLWidget::resizeGL(int width, int height) {}

void GLWidget::mousePressEvent(QMouseEvent *event) {
    lastPos = event->position().toPoint();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event) {
    int dx = event->position().toPoint().x() - lastPos.x();
    int dy = event->position().toPoint().y() - lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        // rotateBy(8 * dy, 8 * dx, 0);
        auto size = qMin(width(), height());
        mesh.explosion(
            vec2d((20.0f / size) * (event->pos().rx() - width() / 2),
                  (20.0f / size) * (height() / 2 - event->pos().ry())));
        update();
    } else if (event->buttons() & Qt::RightButton) {
        rotateBy(8 * dy, 0, 8 * dx);
    }
    lastPos = event->position().toPoint();
}

void GLWidget::mouseReleaseEvent(QMouseEvent * /* event */) { emit clicked(); }
