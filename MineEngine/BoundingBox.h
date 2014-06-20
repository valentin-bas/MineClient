#pragma once

#include "Plane.h"

namespace vox
{
	class BoundingBox
	{
	public:
		enum PointValidity { Invalid = 0, Valid =1};
		enum PlaneIntersectionType { Front = 0, Back = 1, Intersecting = 2};
		PointValidity Validity;

		glm::vec3 min;
		glm::vec3 max;

		BoundingBox();
		BoundingBox(const glm::vec3 &minN, const glm::vec3 &maxN);
		BoundingBox(float minx, float miny, float minz, float maxx, float maxy, float maxz);
		~BoundingBox();

		void expandToInclude(glm::vec3 pt);

		//intersections
		bool contains(glm::vec3 pt);
		bool intersect(BoundingBox &box);
		PlaneIntersectionType intersect (Plane &plane);
		glm::vec3 onBorder(glm::vec3 pt);

		glm::vec3 getCenter() const;
		float getRadius() const;
		float getSize() const;

	};
}