#pragma once

#include "Plane.h"
#include "BoundingBox.h"

namespace vox
{
	class Frustum
	{
	public:
		Frustum();
		virtual ~Frustum();
		enum
		{
			FRUSTUM_PLANE_LEFT   = 0,
			FRUSTUM_PLANE_RIGHT  = 1,
			FRUSTUM_PLANE_BOTTOM = 2,
			FRUSTUM_PLANE_TOP    = 3,
			FRUSTUM_PLANE_NEAR   = 4,
			FRUSTUM_PLANE_FAR    = 5
		};

		enum PlaneIntersectionType { Outside = 0, Inside = 1, Intersects = 2 };

		Plane planes[6];

		void ExtractPlanes(glm::mat4 &clip);

		bool BBoxInFrustum(const BoundingBox &box) const;
		bool PointInFrustum(const glm::vec3 &point) const;
		bool PointInFrustum(int plane,const glm::vec3 &point,float &distance) const;

		//testing
		PlaneIntersectionType BoxInFrustum(BoundingBox &box);
		PlaneIntersectionType BoxInFrustum(float minx,float miny,float minz,float maxx,float maxy,float maxz);
	};
}