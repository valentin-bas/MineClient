#include "stdafx.h"
#include "Frustum.h"

using namespace vox;

Frustum::Frustum(void)
{
}


Frustum::~Frustum(void)
{
}


void Frustum::ExtractPlanes(glm::mat4 &clip)
{
	
        Plane *pPlane = 0;
		pPlane = &planes[FRUSTUM_PLANE_NEAR];
		pPlane->set(clip[0].w + clip[0].z, clip[1].w + clip[1].z, clip[2].w + clip[2].z, clip[3].w + clip[3].z);
        pPlane->normalize();

        // Left clipping plane.
        pPlane = &planes[FRUSTUM_PLANE_FAR];
        pPlane->set(clip[0].w - clip[0].z, clip[1].w - clip[1].z,clip[2].w - clip[2].z,clip[3].w - clip[3].z);
        pPlane->normalize();

        // Left clipping plane.
        pPlane = &planes[FRUSTUM_PLANE_LEFT];
        pPlane->set(clip[0].w + clip[0].x,clip[1].w + clip[1].x,clip[2].w + clip[2].x,clip[3].w + clip[3].x);
        pPlane->normalize();

        pPlane = &planes[FRUSTUM_PLANE_RIGHT];
        pPlane->set(clip[0].w - clip[0].x, clip[1].w - clip[1].x, clip[2].w - clip[2].x, clip[3].w - clip[3].x);
        pPlane->normalize();

        pPlane = &planes[FRUSTUM_PLANE_BOTTOM];
        pPlane->set(clip[0].w + clip[0].y,clip[1].w + clip[1].y,clip[2].w + clip[2].y,clip[3].w + clip[3].y);
        pPlane->normalize();

        pPlane = &planes[FRUSTUM_PLANE_TOP];
        pPlane->set(clip[0].w - clip[0].y, clip[1].w - clip[1].y, clip[2].w - clip[2].y, clip[3].w- clip[3].y);
        pPlane->normalize();
}

Frustum::PlaneIntersectionType Frustum::BoxInFrustum(float minx,float miny,float minz,float maxx,float maxy,float maxz)
{
        BoundingBox box(minx,miny,minz,maxx,maxy,maxz);
        bool flag = false;

        for (int i = 0; i < 6; ++i)
        {
                switch(box.intersect(planes[i]))
                {
                    case BoundingBox::Back:
                    return Outside;

                    case BoundingBox::Front:
                    case BoundingBox::Intersecting:
                        flag = true;
                        break;
                }
        }

        if (!flag)
        {
                return Inside;
        }


        return Intersects;
}

Frustum::PlaneIntersectionType Frustum::BoxInFrustum(BoundingBox &box)
{
    int flag = 0;
    for (int i = 0; i < 6; ++i)
    {
            switch(box.intersect(planes[i]))
            {
                    case BoundingBox::Front:
                    case BoundingBox::Intersecting:
                    {
                            flag = 1;
                    }break;

                    case BoundingBox::Back:
                    return Outside;
            }
    }

    return Intersects;
}

 bool Frustum::BBoxInFrustum(const BoundingBox &box) const
{
	glm::vec3 c((box.min + box.max) * 0.5f);
	float sizex = box.max.x - box.min.x;
	float sizey = box.max.y - box.min.y;
	float sizez = box.max.z - box.min.z;

	glm::vec3 corners[8] =
	{
		glm::vec3((c.x - sizex * 0.5f), (c.y - sizey * 0.5f), (c.z - sizez * 0.5f)),
		glm::vec3((c.x + sizex * 0.5f), (c.y - sizey * 0.5f), (c.z - sizez * 0.5f)),
		glm::vec3((c.x - sizex * 0.5f), (c.y + sizey * 0.5f), (c.z - sizez * 0.5f)),
		glm::vec3((c.x + sizex * 0.5f), (c.y + sizey * 0.5f), (c.z - sizez * 0.5f)),
		glm::vec3((c.x - sizex * 0.5f), (c.y - sizey * 0.5f), (c.z + sizez * 0.5f)),
		glm::vec3((c.x + sizex * 0.5f), (c.y - sizey * 0.5f), (c.z + sizez * 0.5f)),
		glm::vec3((c.x - sizex * 0.5f), (c.y + sizey * 0.5f), (c.z + sizez * 0.5f)),
		glm::vec3((c.x + sizex * 0.5f), (c.y + sizey * 0.5f), (c.z + sizez * 0.5f))
	};

	for (int i = 0; i < 6; ++i)
	{
		if (Plane::dot(planes[i], corners[0]) > 0.0f)
				continue;

		if (Plane::dot(planes[i], corners[1]) > 0.0f)
				continue;

		if (Plane::dot(planes[i], corners[2]) > 0.0f)
				continue;

		if (Plane::dot(planes[i], corners[3]) > 0.0f)
				continue;

		if (Plane::dot(planes[i], corners[4]) > 0.0f)
				continue;

		if (Plane::dot(planes[i], corners[5]) > 0.0f)
				continue;

		if (Plane::dot(planes[i], corners[6]) > 0.0f)
				continue;

		if (Plane::dot(planes[i], corners[7]) > 0.0f)
				continue;

		return false;
	}

	return true;
}

bool Frustum::PointInFrustum(const glm::vec3 &point) const
{
	for (int i = 0; i < 6; ++i)
	{
		if (Plane::dot(planes[i], point) <= 0.0f)
			return false;
	}

	return true;
}

bool Frustum::PointInFrustum(int plane,const glm::vec3 &point,float &distance) const
{
	distance = Plane::dot(planes[plane], point);
	if (distance <= 0.0f)
		return false;

	return true;
}