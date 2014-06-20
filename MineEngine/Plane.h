#pragma once

namespace vox
{
	class Plane
	{
	public:
		Plane();
		~Plane();
		glm::vec3 n;
		float d;

		static float dot(const Plane &p, const glm::vec3 &pt);
		Plane(float a, float b, float c, float d);
		Plane(const glm::vec3 &pt, const glm::vec3 &normal);
		Plane(const glm::vec3 &pt1, const glm::vec3 &pt2, const glm::vec3 &pt3);
		const glm::vec3 &normal() const;
		glm::vec3 &normal();
		void normalize();
		void set(float a, float b, float c, float d);
		void fromPointNormal(const glm::vec3 &pt, const glm::vec3 &normal);
		void fromPoints(const glm::vec3 &pt1, const glm::vec3 &pt2, const glm::vec3 &pt3);
	};
}