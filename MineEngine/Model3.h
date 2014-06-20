#ifndef __MODEL3_H__
#define __MODEL3_H__

#include "Obj.h"
#include "Entity.h"

namespace vox
{
	class	Model3 : public Entity
	{
	public:
		Model3();
		virtual ~Model3();

		bool	Init(id::Obj* obj);
		bool	Render(GLint attrCoord3D, GLint attrVertexUV,
					   GLint attrVertexNormal, GLint attrUniformModel, GLint attrTexture);

	private:
		id::Obj*		_obj;
	};
}

#endif
