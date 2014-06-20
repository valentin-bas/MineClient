#include "stdafx.h"
#include <vector>
#include "TextRenderer.h"
#include "Image_Utils.h"
#include <fstream>
#include <string>

vox::TextRenderer::TextRenderer(void)
{
}


vox::TextRenderer::~TextRenderer(void)
{
	glDeleteBuffers(1, &_VertexBuffer);
	glDeleteBuffers(1, &_UVBuffer);
	
	glDeleteTextures(1, &_texture);
	
	glDeleteProgram(_program);
}

bool
vox::TextRenderer::Init(int width, int height)
{
	_width = width;
	_height = height;
	if (InitShader() == false)
		return false;
	_texture = loadDDS("ArialFont.dds");
	if (_texture <= 0)
		return false;

	if (InitCharWidth() == false)
		return false;
	glGenSamplers(1, &_sampler);
	glBindSampler(0, _sampler);
	glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(_sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glSamplerParameteri(_sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glSamplerParameteri(_sampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16);
	glBindSampler(0, 0);

	glGenBuffers(1, &_VertexBuffer);
	glGenBuffers(1, &_UVBuffer);
	return true;
}

bool
vox::TextRenderer::InitCharWidth(void)
{
	_charHeight = 20;
	_charWidth = new (std::nothrow) char[256];
	if (_charWidth == nullptr)
		return false;
	std::ifstream	file("ArialFontdata.csv");
	char			buff[2048];

	if (file.is_open() == false)
		return false;
	file.getline(buff, 2048);
	std::string	str(buff);
	for (int i = 255; i >= 0; --i)
	{
		std::string	nbr;
		int			j;
		for(j = 0; str[str.length() - j] != ','; ++j)
				;
		nbr = str.substr(str.length()  - j + 1, j);
		str.erase(str.length()  - j, j + 1);
		_charWidth[i] = std::stoi(nbr, nullptr, 10);
	}
	file.close();
	return true;
}

bool
vox::TextRenderer::InitShader(void)
{
	if (CreateProgram("text.v.glsl", "text.f.glsl", &_program) == false)
		return false;

	attrCoord3D = glGetAttribLocation(_program, "coord3d");
	attrVertexUV = glGetAttribLocation(_program, "vertexUV");
	if (attrCoord3D == -1 ||
	    attrVertexUV == -1)
		return false;
	return true;
}

bool
vox::TextRenderer::RenderText(const std::string& text, Vector2Int& pos, int size)
{
	std::vector<glm::vec2>	vertices;
	std::vector<glm::vec2>	UV;
	float					offset = 0;
	
	if (text.empty())
		return false;
	
	for (unsigned int i = 0 ; i < text.length() ; i++)
	{
		float ratio = (float)_charWidth[i] / (float)_charHeight;
		float width = (float)size * ratio;

		glm::vec2 vertex_up_left    = glm::vec2(pos.x + offset		  , pos.y + size);
		glm::vec2 vertex_up_right   = glm::vec2(pos.x +	offset + width, pos.y + size);
		glm::vec2 vertex_down_right = glm::vec2(pos.x +	offset + width, pos.y		 );
		glm::vec2 vertex_down_left  = glm::vec2(pos.x + offset		  , pos.y		 );
		
		vertices.push_back(vertex_up_left);
		vertices.push_back(vertex_down_left);
		vertices.push_back(vertex_up_right);
		
		vertices.push_back(vertex_down_right);
		vertices.push_back(vertex_up_right);
		vertices.push_back(vertex_down_left);

		float uv_x = (text[i] % 16) / 16.0f;
		float uv_y = (text[i] / 16) / 16.0f;

		glm::vec2 uv_up_left    = glm::vec2(uv_x						 , uv_y);
		glm::vec2 uv_up_right   = glm::vec2(uv_x + (1.0f / 16.0f) * ratio, uv_y);
		glm::vec2 uv_down_right = glm::vec2(uv_x + (1.0f / 16.0f) * ratio, (uv_y + 1.0f / 16.0f));
		glm::vec2 uv_down_left  = glm::vec2(uv_x						 , (uv_y + 1.0f / 16.0f));
		
		UV.push_back(uv_up_left);
		UV.push_back(uv_down_left);
		UV.push_back(uv_up_right);
		
		UV.push_back(uv_down_right);
		UV.push_back(uv_up_right);
		UV.push_back(uv_down_left);
		offset += width;
	}
	glViewport(0, 0, _width, _height);
	glUseProgram(_program);
	glActiveTexture(GL_TEXTURE0);
	glBindSampler(0, _sampler);
	glBindTexture(GL_TEXTURE_2D, _texture);

	glBindBuffer(GL_ARRAY_BUFFER, _VertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec2), &vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, _UVBuffer);
	glBufferData(GL_ARRAY_BUFFER, UV.size() * sizeof(glm::vec2), &UV[0], GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(attrCoord3D);
	glBindBuffer(GL_ARRAY_BUFFER, _VertexBuffer);
	glVertexAttribPointer(attrCoord3D, 2, GL_FLOAT, GL_FALSE, 0, (void*)0 );
	
	glEnableVertexAttribArray(attrVertexUV);
	glBindBuffer(GL_ARRAY_BUFFER, _UVBuffer);
	glVertexAttribPointer(attrVertexUV, 2, GL_FLOAT, GL_FALSE, 0, (void*)0 );

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glDrawArrays(GL_TRIANGLES, 0, vertices.size() );
	glDisable(GL_BLEND);
	
	glDisableVertexAttribArray(attrCoord3D);
	glDisableVertexAttribArray(attrVertexUV);
	
	return true;
}
