#include "stdafx.h"
#include "Plane.h"

using namespace vox;

Plane::Plane(void)
{
}

Plane::Plane(float a, float b, float c, float d) : n(a,b,c), d(d)
{
}

Plane::Plane(const glm::vec3 &pt, const glm::vec3 &normal)
{
	fromPointNormal(pt, normal);
}

Plane::~Plane(void)
{
}

Plane::Plane(const glm::vec3 &pt1, const glm::vec3 &pt2, const glm::vec3 &pt3)
{
        fromPoints(pt1, pt2, pt3);
}


float Plane::dot(const Plane &p, const glm::vec3 &pt)
{
        // Returns:
        //  > 0 if the point 'pt' lies in front of the plane 'p'
        //  < 0 if the point 'pt' lies behind the plane 'p'
        //    0 if the point 'pt' lies on the plane 'p'
        //
        // The signed distance from the point 'pt' to the plane 'p' is returned.

        return glm::dot(p.n, pt) + p.d;
}

/*
bool Plane::operator==(const Plane &rhs) const
{
        return (n == rhs.n) && Math::closeEnough(d, rhs.d);
}

bool Plane::operator!=(const Plane &rhs) const
{
        return !(*this == rhs);
}
*/

void Plane::fromPointNormal(const glm::vec3 &pt, const glm::vec3 &normal)
{
        set(normal.x, normal.y, normal.z, -glm::dot(normal, pt));
        normalize();
}

void Plane::fromPoints(const glm::vec3 &pt1, const glm::vec3 &pt2, const glm::vec3 &pt3)
{
        n = glm::cross(pt2 - pt1, pt3 - pt1);
        d = -glm::dot(n, pt1);
        normalize();
}

const glm::vec3 &Plane::normal() const
{
        return n;
}

glm::vec3 &Plane::normal()
{
        return n;
}

void Plane::normalize()
{
		float length = 1.0f / n.length();
        n *= length;
        d *= length;
}

void Plane::set(float a, float b, float c, float d)
{
        n.x = a;
		n.y = b;
		n.z = c;
        this->d = d;
}