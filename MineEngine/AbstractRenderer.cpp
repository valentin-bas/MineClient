#include "stdafx.h"
#include "AbstractRenderer.h"
#include "shader_utils.h"
#include "Image_Utils.h"

vox::AbstractRenderer::AbstractRenderer(void)
{
}


vox::AbstractRenderer::~AbstractRenderer(void)
{
}

bool
vox::AbstractRenderer::Init(int width, int height)
{
	_width = width;
	_height = height;
	if (AbstractRenderer::InitShader() == false)
		return false;
	glGenTextures(1, &renderTexture);
	glBindTexture(GL_TEXTURE_2D, renderTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &renderTextureOdd);
	glBindTexture(GL_TEXTURE_2D, renderTextureOdd);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTexture, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return false;	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenSamplers(1, &sampler);
	glBindSampler(0, sampler);
	glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(sampler, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glSamplerParameteri(sampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16);
	glBindSampler(0, 0);

	GLfloat vertexBufferData[] = {
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    1.0f,  1.0f, 0.0f	};


	glGenBuffers(1, &PostProcessVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, PostProcessVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBufferData), vertexBufferData, GL_STATIC_DRAW);

//	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	glEnable(GL_CULL_FACE);
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);				// Set out clear colour to black, full alpha
	glEnable(GL_DEPTH_TEST);							// Enable the depth buffer
	glClearDepth(1.0f);									// Clear the entire depth of the depth buffer
	glDepthFunc(GL_LEQUAL);								// Set our depth function to overwrite if new value less than or equal to current value
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Ask for nicest perspective correction
	glLineWidth(1.0f);
	return true;
}

bool
vox::AbstractRenderer::InitShader(void)
{
	if (CreateProgram("cube.v.glsl", "cube.f.glsl", &program) == false ||
		CreateProgram("cubeAA.v.glsl", "cubeAA.f.glsl", &AAProgram) == false)
	return false;

	attrCoord3D = glGetAttribLocation(program, "coord3d");
	attrVertexUV = glGetAttribLocation(program, "vertexUV");
	attrVertexNormal = glGetAttribLocation(program, "vertexNormal");
	attrUniformViewProj = glGetUniformLocation(program, "ViewProj");
	attrUniformModel = glGetUniformLocation(program, "Model");
	attrTime = glGetUniformLocation(program, "myTime");
	attrTexture = glGetUniformLocation(program, "myTextureSampler");
	if (attrCoord3D == -1 ||
	    attrVertexUV == -1 ||
	    attrUniformViewProj == -1 ||
		attrUniformModel == -1 || attrTexture == -1 ||
		attrVertexNormal == -1 || attrTime == -1)
		return false;
	return true;
}