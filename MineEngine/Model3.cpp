#include "stdafx.h"
#include "Model3.h"

vox::Model3::Model3()
{
	pos.x = 0.0f;
	pos.y = 0.0f;
	pos.z = 0.0f;
	rot.x = 0.0f;
	rot.y = 0.0f;
	rot.z = 0.0f;
}

vox::Model3::~Model3()
{
}

bool
vox::Model3::Init(id::Obj* obj)
{
	_obj = obj;
	return true;
}

bool
vox::Model3::Render(GLint attrCoord3D, GLint attrVertexUV,
				   GLint attrVertexNormal, GLint attrUniformModel, GLint attrTexture)
{
	glm::mat4 Model = glm::translate(glm::mat4(), pos) * glm::rotate(rot.y, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(attrUniformModel, 1, GL_FALSE, &Model[0][0]);
	glEnableVertexAttribArray(attrCoord3D);
	GLuint vboVertex, vboNormal, vboUV, texture;
	unsigned int triangleCount;
	_obj->GetRenderInfo(texture, vboVertex, vboUV, vboNormal, triangleCount);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(attrTexture, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vboVertex);
	glVertexAttribPointer(
		attrCoord3D,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	
	glEnableVertexAttribArray(attrVertexNormal);
	glBindBuffer(GL_ARRAY_BUFFER, vboNormal);
	glVertexAttribPointer(
		attrVertexNormal,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	
	glEnableVertexAttribArray(attrVertexUV);
	glBindBuffer(GL_ARRAY_BUFFER, vboUV);
	glVertexAttribPointer(
	    attrVertexUV,                                // attribute. No particular reason for 1, but must match the layout in the shader.
	    2,                                // size
	    GL_FLOAT,                         // type
	    GL_FALSE,                         // normalized?
	    0,                                // stride
	    (void*)0                          // array buffer offset
	);

	glDrawArrays(GL_TRIANGLES, 0, triangleCount * 3);

	glDisableVertexAttribArray(attrVertexUV);
	glDisableVertexAttribArray(attrVertexNormal);
	glDisableVertexAttribArray(attrCoord3D);
	return true;
}