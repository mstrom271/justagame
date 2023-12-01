#pragma once

#include <QPointF>
#include <QTransform>

constexpr double pi = 3.14159265358979323846;
double toRadians(double degrees);
double toDegrees(double radians);

class mat23;

class vec2d {
    double ix, iy;

  public:
    vec2d();
    vec2d(const vec2d &v) = default;
    vec2d &operator=(const vec2d &v) = default;
    vec2d(vec2d &&v) noexcept = default;
    vec2d &operator=(vec2d &&v) noexcept = default;
    ~vec2d() = default;

    vec2d(double x, double y);
    vec2d(double angle); // normalized vector

    vec2d(const QPointF &p);
    vec2d &operator=(const QPointF &p);
    operator QPointF() const;

    double x() const;
    void setX(double newX);
    double y() const;
    void setY(double newY);

    vec2d &rotate(double angle);
    vec2d rotated(double angle) const;
    vec2d &translate(double x, double y);
    vec2d translated(double x, double y) const;
    vec2d &norm();
    vec2d normed() const;
    vec2d &perp(); //+90 degrees
    vec2d perped() const;
    double dotProduct(const vec2d &v) const;
    double det(const vec2d &v) const;
    double angle(const vec2d &v) const;
    double length() const;
    double length2() const;
    double manhattanLength() const;

    vec2d operator+(const vec2d &v) const;
    vec2d &operator+=(const vec2d &v);
    vec2d operator-(const vec2d &v) const;
    vec2d &operator-=(const vec2d &v);
    vec2d operator-() const;
    vec2d operator*(double scale) const;
    vec2d &operator*=(double scale);
    vec2d operator*(const mat23 &m) const;
    vec2d &operator*=(const mat23 &m);
    double operator*(const vec2d &v) const;
    vec2d operator/(double divisor) const;
    vec2d &operator/=(double divisor);

    friend mat23;
};

class mat23 {
    double im11, im12;
    double im21, im22;
    double im31, im32;

  public:
    mat23(); // identity matrix
    mat23(double m11, double m12, double m21, double m22, double m31,
          double m32);
    mat23(const vec2d &v1, const vec2d &v2);

    mat23(const mat23 &v) = default;
    mat23 &operator=(const mat23 &v) = default;
    mat23(mat23 &&v) noexcept = default;
    mat23 &operator=(mat23 &&v) noexcept = default;
    ~mat23() = default;

    operator QTransform() const;
    double det() const; // check correctness
    mat23 operator*(const mat23 &m) const;
    mat23 &operator*=(const mat23 &m);

    mat23 &identity();
    mat23 &translate(const vec2d &dir);
    mat23 &translate(double x, double y);
    mat23 translated(const vec2d &dir) const;
    mat23 translated(double x, double y) const;
    mat23 &rotate(double angle);
    mat23 rotated(double angle) const;
    mat23 &scale(double scale);
    mat23 &scale(double s1, double s2);
    mat23 scaled(double scale) const;
    mat23 scaled(double s1, double s2) const;

    static mat23 makeIdentity();
    static mat23 makeTranslate(const vec2d &dir);
    static mat23 makeRotate(double angle);
    static mat23 makeScale(double scale);
    static mat23 makeScale(double s1, double s2);

    friend vec2d;
};
