#include "math2d.h"
#include <QDebug>
#include <cmath>

double toRadians(double degrees) { return degrees / 360 * 2 * pi; }

double toDegrees(double radians) { return radians / (2 * pi) * 360; }

vec2d::vec2d(double x, double y) : ix(x), iy(y) {}

vec2d::vec2d(double angle) {
    ix = std::cos(angle);
    iy = std::sin(angle);
}

vec2d::vec2d() : ix(0), iy(0) {}

vec2d::vec2d(const QPointF &p) {
    ix = p.x();
    iy = p.y();
}

vec2d &vec2d::operator=(const QPointF &p) {
    ix = p.x();
    iy = p.y();
    return *this;
}

vec2d::operator QPointF() const { return QPointF(ix, iy); }

double vec2d::x() const { return ix; }
void vec2d::setX(double newX) { ix = newX; }
double vec2d::y() const { return iy; }
void vec2d::setY(double newY) { iy = newY; }

vec2d &vec2d::rotate(double angle) {
    double ca = std::cos(angle);
    double sa = std::sin(angle);
    *this = vec2d(ix * ca - iy * sa, ix * sa + iy * ca);
    return *this;
}

vec2d vec2d::rotated(double angle) const {
    double ca = std::cos(angle);
    double sa = std::sin(angle);
    return vec2d(ix * ca - iy * sa, ix * sa + iy * ca);
}

vec2d &vec2d::translate(double x, double y) {
    *this += vec2d(x, y);
    return *this;
}

vec2d vec2d::translated(double x, double y) const {
    return *this + vec2d(x, y);
}

vec2d &vec2d::norm() {
    *this /= length();
    return *this;
}

vec2d vec2d::normed() const { return *this / length(); }

vec2d &vec2d::perp() { //+90 degrees
    *this = vec2d(-iy, ix);
    return *this;
}

vec2d vec2d::perped() const { return vec2d(-iy, ix); }

double vec2d::dotProduct(const vec2d &v) const { return ix * v.ix + iy * v.iy; }

double vec2d::det(const vec2d &v) const { return ix * v.iy - iy * v.ix; }

double vec2d::angle(const vec2d &v) const {
    // atan2(y, x) or atan2(sin, cos)
    return -std::atan2(det(v), dotProduct(v));
}

double vec2d::length() const { return std::sqrt(ix * ix + iy * iy); }

double vec2d::length2() const { return ix * ix + iy * iy; }

double vec2d::manhattanLength() const { return std::abs(ix) + std::abs(iy); }

vec2d vec2d::operator+(const vec2d &v) const {
    return vec2d(ix + v.ix, iy + v.iy);
}

vec2d &vec2d::operator+=(const vec2d &v) {
    ix += v.ix;
    iy += v.iy;
    return *this;
}

vec2d vec2d::operator-(const vec2d &v) const {
    return vec2d(ix - v.ix, iy - v.iy);
}

vec2d &vec2d::operator-=(const vec2d &v) {
    ix -= v.ix;
    iy -= v.iy;
    return *this;
}

vec2d vec2d::operator*(const double scale) const {
    return vec2d(ix * scale, iy * scale);
}

vec2d &vec2d::operator*=(double scale) {
    ix *= scale;
    iy *= scale;
    return *this;
}

vec2d vec2d::operator*(const mat23 &m) const {
    return vec2d(ix * m.im11 + iy * m.im21 + 1 * m.im31,
                 ix * m.im12 + iy * m.im22 + 1 * m.im32);
}

vec2d &vec2d::operator*=(const mat23 &m) {
    *this = vec2d(ix * m.im11 + iy * m.im21 + 1 * m.im31,
                  ix * m.im12 + iy * m.im22 + 1 * m.im32);
    return *this;
}

double vec2d::operator*(const vec2d &v) const { return ix * v.ix + iy * v.iy; }

vec2d vec2d::operator/(double divisor) const {
    return vec2d(ix / divisor, iy / divisor);
}

vec2d &vec2d::operator/=(double divisor) {
    ix /= divisor;
    iy /= divisor;
    return *this;
}

mat23::mat23() : im11(1), im12(0), im21(0), im22(1), im31(0), im32(0) {}

mat23::mat23(double m11, double m12, double m21, double m22, double m31,
             double m32)
    : im11(m11), im12(m12), im21(m21), im22(m22), im31(m31), im32(m32) {}

mat23::mat23(const vec2d &v1, const vec2d &v2)
    : im11(v1.x()), im12(v2.x()), im21(v1.y()), im22(v2.y()), im31(0), im32(0) {
}

mat23::operator QTransform() const {
    return QTransform(im11, im12, 0, im21, im22, 0, im31, im32, 1);
}

double mat23::det() const {
    // check correctness. Is this formula applicable for 2x3 matrix? Seems that
    // im31 and im32 members don't affect determinant
    return im11 * im22 - im12 * im21;
}

mat23 mat23::operator*(const mat23 &m) const {
    return mat23(im11 * m.im11 + im12 * m.im21, im11 * m.im12 + im12 * m.im22,
                 im21 * m.im11 + im22 * m.im21, im21 * m.im12 + im22 * m.im22,
                 im31 * m.im11 + im32 * m.im21 + 1 * m.im31,
                 im31 * m.im12 + im32 * m.im22 + 1 * m.im32);
}

mat23 &mat23::operator*=(const mat23 &m) {
    *this = mat23(im11 * m.im11 + im12 * m.im21, im11 * m.im12 + im12 * m.im22,
                  im21 * m.im11 + im22 * m.im21, im21 * m.im12 + im22 * m.im22,
                  im31 * m.im11 + im32 * m.im21 + 1 * m.im31,
                  im31 * m.im12 + im32 * m.im22 + 1 * m.im32);
    return *this;
}

mat23 &mat23::identity() {
    *this = mat23(1, 0, 0, 1, 0, 0);
    return *this;
}

mat23 &mat23::translate(const vec2d &dir) {
    im31 += dir.ix;
    im32 += dir.iy;
    return *this;
}

mat23 &mat23::translate(double x, double y) {
    im31 += x;
    im32 += y;
    return *this;
}

mat23 mat23::translated(const vec2d &dir) const {
    return mat23(im11, im12, im21, im22, im31 + dir.ix, im32 + dir.iy);
}

mat23 mat23::translated(double x, double y) const {
    return mat23(im11, im12, im21, im22, im31 + x, im32 + y);
}

mat23 &mat23::rotate(double angle) {
    *this *= makeRotate(angle);
    return *this;
}

mat23 mat23::rotated(double angle) const { return *this * makeRotate(angle); }

mat23 &mat23::scale(double scale) {
    *this = mat23(im11 * scale, im12 * scale, im21 * scale, im22 * scale,
                  im31 * scale, im32 * scale);
    return *this;
}

mat23 &mat23::scale(double s1, double s2) {
    *this =
        mat23(im11 * s1, im12 * s2, im21 * s1, im22 * s2, im31 * s1, im32 * s2);
    return *this;
}

mat23 mat23::scaled(double scale) const {
    return mat23(im11 * scale, im12 * scale, im21 * scale, im22 * scale,
                 im31 * scale, im32 * scale);
}

mat23 mat23::scaled(double s1, double s2) const {
    return mat23(im11 * s1, im12 * s2, im21 * s1, im22 * s2, im31 * s1,
                 im32 * s2);
}

mat23 mat23::makeTranslate(const vec2d &dir) {
    return mat23(1, 0, 0, 1, dir.ix, dir.iy);
}

mat23 mat23::makeRotate(double angle) {
    return mat23(std::cos(angle), std::sin(angle), -std::sin(angle),
                 std::cos(angle), 0, 0);
}

mat23 mat23::makeScale(double scale) { return mat23(scale, 0, 0, scale, 0, 0); }

mat23 mat23::makeScale(double s1, double s2) {
    return mat23(s1, 0, 0, s2, 0, 0);
}

mat23 mat23::makeIdentity() { return mat23(1, 0, 0, 1, 0, 0); }
