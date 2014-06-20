#pragma once

namespace vox
{
	class Entity
	{
	public:
						Entity(void);
		virtual			~Entity(void);

		virtual bool	Render(GLint attrCoord3D, GLint attrVertexColor,
							   GLint attrVertexNormal, GLint attrUniformModel, GLint attrTexture) = 0;

		glm::vec3	pos;
		glm::vec3	rot;
	};
}