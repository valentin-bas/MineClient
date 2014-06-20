#include "stdafx.h"
#include "BoundingBox.h"

using namespace vox;

BoundingBox::BoundingBox(void)
{
	min= glm::vec3(0.0f, 0.0f, 0.0f);
	max = glm::vec3(0.0f, 0.0f, 0.0f);
	Validity = BoundingBox::Invalid;
}

BoundingBox::BoundingBox(const glm::vec3 &min, const glm::vec3 &max)
{
        this->min = min;
        this->max= max;
        Validity = BoundingBox::Valid;
}

BoundingBox::BoundingBox(float minx,float miny,float minz,float maxx,float maxy,float maxz)
{
        min.x = minx;
        min.y = miny;
        min.z = minz;

        max.x = maxx;
        max.y = maxy;
        max.z = maxz;

        Validity = BoundingBox::Valid;
}

BoundingBox::~BoundingBox(void)
{
}

void BoundingBox::expandToInclude(glm::vec3 pt)
{
	if (Validity == BoundingBox::Valid)
	{
		if (pt.x < min.x) { min.x = pt.x; }
		else if (pt.x > max.x) { max.x = pt.x; }

		if (pt.y < min.y) { min.y = pt.y; }
		else if (pt.y > max.y) { max.y = pt.y; }

		if (pt.z < min.z) { min.z = pt.z; }
		else if (pt.z > max.z) { max.z = pt.z; }
	}
	else
	{
		min = max = pt;
		Validity = BoundingBox::Valid;
	}
}

bool BoundingBox::contains(glm::vec3 pt)
{
	return ((pt.x >= min.x) && (pt.x <= max.x) && (pt.y >= min.y) && (pt.y <= max.y) && (pt.z >= min.z) && (pt.z <= max.z));
}

bool BoundingBox::intersect(BoundingBox &box)
{
	if (!((min.x <= box.max.x) && (max.x >= box.min.x)))
		return false;

	if (!((min.y <= box.max.y) && (max.y >= box.min.y)))
		return false;

	return ((min.z <= box.max.z) && (max.z >= box.min.z));
}

 BoundingBox::PlaneIntersectionType BoundingBox::intersect(Plane &plane)
{
	glm::vec3 vector;
	glm::vec3 vector2;
	vector2.x = (plane.n.x >= 0.0f) ? min.x : max.x;
	vector2.y = (plane.n.y >= 0.0f) ? min.y : max.y;
	vector2.z = (plane.n.z >= 0.0f) ? min.z : max.z;
	vector.x = (plane.n.x >= 0.0f) ? max.x : min.x;
	vector.y = (plane.n.y >= 0.0f) ? max.y : min.y;
	vector.z = (plane.n.z >= 0.0f) ? max.z : min.z;


	float num = glm::dot(plane.n,vector2);
	if ((num + plane.d) >= 0.0f)
	{
		return Front;
	}

	num = glm::dot(plane.n,vector);
	if ((num + plane.d) <= 0.0f)
	{
		return Back;
	}

	return Intersecting;
}

glm::vec3 BoundingBox::onBorder(glm::vec3 pt)
{
	glm::vec3 result(0,0,0);

    int x = (int)(min.x - pt.x);
    if(x == 0)
            result.x = -1;

    x = (int)(max.x - pt.x);
    if(x == 0)
            result.x = 1;

    int y =  (int)(min.y - pt.y);
    if(y == 0)
            result.y = -1;

    y = (int)(max.y - pt.y);
    if(y == 0)
            result.y = 1;

    int z =  (int)(min.z - pt.z);
    if(z == 0)
            result.z = -1;

    z = (int)(max.z - pt.z);
    if(z == 0)
            result.z = 1;


    return result;
}

glm::vec3 BoundingBox::getCenter() const
{
	return (min + max) * 0.5f;
}

float BoundingBox::getRadius() const
{
	return getSize() * 0.5f;
}

float BoundingBox::getSize() const
{
	return (float)(max - min).length();
}