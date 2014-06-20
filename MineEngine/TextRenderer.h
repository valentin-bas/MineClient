#pragma once

#include "Vector2Int.h"

namespace vox
{
	class TextRenderer
	{
		//Window Properties
		int			_width;
		int			_height;

		//Shaders
		GLuint		_program;
		GLint		attrCoord3D;
		GLint		attrVertexUV;

		GLuint		_texture;
		GLuint		_sampler;

		GLuint		_VertexBuffer;
		GLuint		_UVBuffer;

		char*		_charWidth;
		char		_charHeight;

		bool	InitShader(void);
		bool	InitCharWidth(void);
	public:
				TextRenderer(void);
				~TextRenderer(void);

		bool	Init(int width, int height);
		bool	RenderText(const std::string& text, Vector2Int& position, int size);
	};
}