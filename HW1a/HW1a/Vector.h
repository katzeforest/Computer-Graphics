#pragma once

#include <iostream>
#include <cmath>

class Vector
{
private:
    double m_values[3];

public:
    Vector(double x, double y, double z);
	Vector(const Vector& other);
	Vector();
    ~Vector(void);

    Vector operator+(Vector &other);
    Vector& operator+=(Vector &other);

	Vector operator-(Vector &other);
	Vector& operator-=(Vector &other);

	Vector& operator=(Vector &other);

	Vector operator*(Vector &other);		// dot product
	Vector& operator*=(Vector &other);	// dot product
	
	Vector operator*(double val);
	Vector& operator*=(double val);

	double& operator[](unsigned int pos);			// [0] = x, [1] = y, [2] = z
	const double& operator[](unsigned int pos) const;// [0] = x, [1] = y, [2] = z

	bool operator==(Vector &other);

	double length();
	Vector normalize();

	Vector crossProduct(Vector &other);	// cross product

	friend std::ostream& operator<<(std::ostream &os, const Vector &v);
};