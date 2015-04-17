#include "Vector.h"
#include <math.h>

Vector::Vector() : x(1.0), y(1.0), z(1.0) {
	normalize();
}
Vector::Vector(float x, float y, float z) : x(x), y(y), z(z) {}

float Vector::length() {
	return sqrt(x*x + y*y + z*z);
}

void Vector::normalize() {
	if (length() != 0.0) {
		float len = length();
		x = x/len;
		y = y/len;
		z = z/len;
	}
}