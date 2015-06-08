#include "Vector.h"

Vector::Vector(double x, double y, double z)
{
    m_values[0] = x;
    m_values[1] = y;
    m_values[2] = z;
}

Vector::Vector()
{
    m_values[0] = 0;
    m_values[1] = 0;
    m_values[2] = 0;
}

Vector::Vector(const Vector& other)
{
    m_values[0] = other[0];
    m_values[1] = other[1];
    m_values[2] = other[2];
}


Vector::~Vector(void)
{}

Vector Vector::operator+(Vector &other)
{
	return Vector(m_values[0] + other[0], m_values[1] + other[1], m_values[2] + other[2]);
}

Vector& Vector::operator+=(Vector &other)
{
	m_values[0] += other[0];
	m_values[1] += other[1];
	m_values[2] += other[2];
	return *this;
}

Vector Vector::operator-(Vector &other)
{
	return Vector(m_values[0] - other[0], m_values[1] - other[1], m_values[2] - other[2]);
}

Vector& Vector::operator-=(Vector &other)
{
	m_values[0] -= other[0];
	m_values[1] -= other[1];
	m_values[2] -= other[2];
	return *this;
}

Vector& Vector::operator=(Vector &other)
{
	m_values[0] = other[0];
	m_values[1] = other[1];
	m_values[2] = other[2];
	return *this;
}

Vector Vector::operator*(Vector &other)
{
    return Vector(m_values[0] * other[0], m_values[1] * other[1], m_values[2] * other[2]);
}

Vector& Vector::operator*=(Vector &other)
{
	m_values[0] *= other[0];
	m_values[1] *= other[1];
	m_values[2] *= other[2];
	return *this;
}

Vector Vector::operator*(double val)
{
    return Vector(m_values[0] * val, m_values[1] * val, m_values[2] * val);
}

Vector& Vector::operator*=(double val)
{
    m_values[0] *= val; m_values[1] *= val; m_values[2] *= val; return *this;
}


double Vector::length()
{
    return sqrt(double(m_values[0]*m_values[0] + m_values[1]*m_values[1] + m_values[2]*m_values[2]));
}


Vector Vector::normalize() {
    double length = this->length();
    double x,y,z;
    x = m_values[0] / length;
    y = m_values[1] / length;
    z = m_values[2] / length;
    return Vector(x, y, z);
}

Vector Vector::crossProduct(Vector &other)
{
	double x = m_values[1]*other[2] - m_values[2]*other[1];
	double y = m_values[2]*other[0] - m_values[0]*other[2];
	double z = m_values[0]*other[1] - m_values[1]*other[0];
	return Vector(x, y, z);
}

bool Vector::operator==(Vector &other)
{
	return m_values[0] == other[0] && m_values[1] == other[1] && m_values[2] == other[2];
}

double& Vector::operator[](unsigned int pos)
{
    if (pos > 2)
        return m_values[0];
    return m_values[pos];
}

const double& Vector::operator[](unsigned int pos) const
{
    if (pos > 2)
        return m_values[0];
    return m_values[pos];
}

std::ostream & operator<<(std::ostream &os, const Vector& v)
{
	return os << "( " << v.m_values[0] << ", " << v.m_values[1] << ", " << v.m_values[2] << ")";
}