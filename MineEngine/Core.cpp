#include "stdafx.h"
#include "Chunk.h"
#include "ChunkColumn.h"
#include "Core.h"
#include "Model3.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include "OculusRenderer.h"
#include "BasicRenderer.h"
#include "SkyDome.h"
#include "TextRenderer.h"

#include "particle/sparticules_interface.h"
#include "particle/sparticules_layer.h"
#include "particle/emitters/sparticules_emitter_simple.h"
#include "particle/updaters/sparticules_updater_physics.h"
#include "particle/updaters/sparticules_updater_life.h"
#include "particle/updaters/sparticules_updater_color.h"
#include "particle/updaters/sparticules_updater_zone.h"
#include "particle/updaters/sparticules_updater_collision.h"
#include "particle/updaters/sparticules_updater_death_trigger.h"
#include "particle/renderers/sparticules_renderer_billboard.h"

vox::Core::Core(void) : _oldTime(0.1f), _oculusPlug(false), _particleWorld(nullptr)
{
}

vox::Core::Core(vox::Core const& right)
{
	(void)right;
}

vox::Core::~Core(void)
{
	this->__destroy();
}

bool
vox::Core::init(bool debugOculus)
{
	float fieldOfView;
	float aspectRatio;

	srand ((unsigned int)time(nullptr));
	_debugOculus = debugOculus;
	glfwSetErrorCallback(this->errorCallback);
	if (!glfwInit())
		return false;
	_width = 1024;
	_height = 768;
	_window = glfwCreateWindow(_width, _height, "id_Vox", nullptr, nullptr);
	if (!_window)
		return false;
	glfwMakeContextCurrent(_window);
	glfwSetKeyCallback(_window, handleKeypress);
	glfwSetCursorPosCallback(_window, handleMouseMove);

	glewInit();
	OVR::System::Init();
	_sensorFusion = new OVR::SensorFusion();
	OVR::DeviceManager*	manager = OVR::DeviceManager::Create();
	OVR::HMDDevice*		device = manager->EnumerateDevices<OVR::HMDDevice>().CreateDevice();

	if (device && !_debugOculus)
	{
		_oculusPlug = true;
		_width = 1280;
		_height = 800;
		SwitchToFullScreen(_width, _height);
		_renderer = new (std::nothrow) OculusRenderer();
		if (_renderer == nullptr || _renderer->Init(_width, _height) == false)
			return false;
		device->GetDeviceInfo(&_info);
		OVR::SensorDevice* sensor = device->GetSensor();
		if (sensor)
			_sensorFusion->AttachToSensor(sensor);
		else
			return false;
		dynamic_cast<OculusRenderer*>(_renderer)->SetOculusConfig(_info);
		fieldOfView = (float)(180.0f * (2.0f * atan((_info.VScreenSize / 2) / _info.EyeToScreenDistance)) / M_PI);
		aspectRatio = 0.5f * _width / _height;
	}
	else
	{
		if (_debugOculus)
		{
			fieldOfView = 97.0f;
			aspectRatio = 0.5f * _width / _height;
			_oculusPlug = true;
			_renderer = new (std::nothrow) OculusRenderer();
			if (_renderer == nullptr || _renderer->Init(_width, _height) == false)
				return false;
		}
		else
		{
			fieldOfView = 90.0f;
			_renderer = new (std::nothrow) BasicRenderer();
			if (_renderer == nullptr || _renderer->Init(_width, _height) == false)
				return false;
			aspectRatio = (float)(_width / _height * 2);
		}
	}
	_textRenderer = new (std::nothrow) TextRenderer();
	if (_textRenderer == nullptr || _textRenderer->Init(_width, _height) == false)
		return false;

	cam = new (std::nothrow) Camera(_window, _width, _height);
	if (cam == nullptr)
		return false;
	if (_oculusPlug && !_debugOculus)
		cam->EnableOculus();
	projection = glm::perspective(fieldOfView, aspectRatio, 0.1f, 1000.0f);
	Chunk::initVertex();

	sky = new (std::nothrow) SkyDome;
	if (sky == nullptr || sky->Init() == false)
		return false;
	try { _entities.insert(std::pair<int, Entity*>(453852, sky)); }
	catch (std::bad_alloc) { delete sky; return false; }

	Timer::Internal::Startup();
	_particleWorld = _CreateNewParticleWorld();
	if (_particleWorld == nullptr)
		return false;

	return true;
}

bool
vox::Core::SwitchToFullScreen(int width, int height)
{
	const GLFWvidmode*	modes;
	int					count;

	glfwDestroyWindow(_window);
	if (width == 0 && height == 0)
	{
		modes = glfwGetVideoModes(glfwGetPrimaryMonitor(), &count);
		_width = modes[count - 1].width;
		_height = modes[count - 1].height;
	}
	else
	{
		_width = width;
		_height = height;
	}
	_window = glfwCreateWindow(_width, _height, "id_Vox", glfwGetPrimaryMonitor(), nullptr);
	if (!_window)
		return false;
	glfwMakeContextCurrent(_window);
	glfwSetKeyCallback(_window, handleKeypress);
	glfwSetCursorPosCallback(_window, handleMouseMove);
	return true;
}

bool
vox::Core::__destroy(void)
{
	for (std::map<Vector3Int, Chunk*>::iterator it = _chunkMap.begin(); it != _chunkMap.end(); ++it)
		delete it->second;
	for (std::map<int, Entity*>::iterator it = _entities.begin(); it != _entities.end(); ++it)
		delete it->second;
	delete _particleWorld;
	glfwDestroyWindow(_window);
	glfwTerminate();
	if (cam != nullptr)
		delete cam;
	SAFE_DELETE(_particleWorld);
	return true;
}

bool
vox::Core::run(bool& shouldClose)
{
	float	yaw, pitch, roll;
	double	dt, time = glfwGetTime();

	dt = time - _oldTime;
	_oldTime = time;
	
	_sensorFusion->GetPredictedOrientation().GetEulerAngles<OVR::Axis_Y, OVR::Axis_X, OVR::Axis_Z>(&yaw, &pitch, &roll);
	Core::GetSingleton().GetInputs()->HandleOculusOrientation(yaw, pitch, roll);
	updateVector();
	
	cam->move((float)(dt), _chunkMap);
	
	glm::vec3	camPos;
	cam->getPosition(camPos);
	sky->pos.y = camPos.y;
	sky->pos.x = -camPos.x;
	sky->pos.z = -camPos.z;


	glfwGetFramebufferSize(_window, &_width, &_height);
	_ratio = _width / (float) _height;

	_particleWorld->Update(dt);

	_renderer->Render(_chunkMap, _entities, *_particleWorld, view, projection);
	_textRenderer->RenderText("Particle Count : " + std::to_string(_particleWorld->ParticleCount()), Vector2Int(0, 70), 30);
	_textRenderer->RenderText("x:" + std::to_string(cam->getXPos()) +
							 " y:"+ std::to_string(cam->getYPos()) +
							 " z:"+ std::to_string(cam->getZPos()), Vector2Int(0, 50), 30);
	_textRenderer->RenderText(std::to_string(dt), Vector2Int(0, 30), 30);
	_textRenderer->RenderText("MineClient.", Vector2Int(0, 10), 30);
	_textRenderer->RenderText("by Valentin, Romeo and Vivien", Vector2Int(0, -10), 30);
	glfwSwapBuffers(_window);
	glfwPollEvents();
	shouldClose = glfwWindowShouldClose(_window) != 0;
	return true;
}

void
vox::Core::updateVector()
{
	float	x = -cam->getXRot() * (float)M_PI / 180.0f;
	float	y = -cam->getYRot() * (float)M_PI / 180.0f;

	right.x = -cosf(y);
	right.y = 0;
	right.z = sinf(y);

	lookat.x = sinf(y) * cosf(x);
	lookat.y =			 sinf(x);
	lookat.z = cosf(y) * cosf(x);

	up = glm::cross(right, lookat);

	position.x = cam->getXPos();
	position.y = -cam->getYPos();
	position.z = cam->getZPos();

	position = - position;
	view = glm::lookAt(position, position + (lookat), up);
}

float
vox::Core::fract(float value)
{
	float f = value - floorf(value);

	if (f > 0.5)
		return 1 - f;
	else
		return f;
	lookat.y = sinf(cam->getXRot());
}

void
vox::Core::selectBlock(glm::vec4& res, glm::vec3& cursor, bool build, int id)
{/*
	int			x, y;
	GLfloat		depth;
	glm::mat4	baseProj;
	glm::mat4	diffView;
	glm::vec4	viewport;

	if (_oculusPlug || _debugOculus)
	{
		OVR::Util::Render::StereoConfig* SConfig = dynamic_cast<OculusRenderer*>(_renderer)->GetStereoConfig();
		float halfIPD = SConfig->GetHMDInfo().InterpupillaryDistance * 0.5f;
		baseProj = projection * glm::translate(glm::vec3(SConfig->GetProjectionCenterOffset(), 0, 0));
		diffView = view * glm::translate(glm::vec3(halfIPD, 0, 0));
		viewport = glm::vec4(0, 0, _width / 2, _height);
		x = _width / 4;
		y = _height / 2;
	}
	else
	{
		baseProj = projection;
		diffView = view;
		viewport = glm::vec4(0, 0, _width, _height);
		x = _width / 2;
		y = _height / 2;
	}
	
	glBindFramebuffer(GL_FRAMEBUFFER, _renderer->GetFrameBuffer());
	glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
	
	glm::vec3	winCoord = glm::vec3(x, y, depth);
	glm::vec3	objCoord = glm::unProject(winCoord, diffView, baseProj, viewport);
	glm::vec3	viewDir = glm::normalize(objCoord - position) * 0.1f;
	
	cursor = glm::vec3(objCoord) / 2.0f;
	objCoord += viewDir;
	Vector2Int	key((int)glm::floor(objCoord.x / (float)(Chunk::CHUNK_SIZE * 2)), (int)glm::floor(objCoord.z / (float)(Chunk::CHUNK_SIZE * 2)));
	glm::vec3 chunkPos((int)glm::round(objCoord.x / 2.0f) - key.x * Chunk::CHUNK_SIZE,
					   (int)glm::round(objCoord.y / 2.0f),
					   (int)glm::round(objCoord.z / 2.0f) - key.y * Chunk::CHUNK_SIZE);
	
	if (chunkPos.x == Chunk::CHUNK_SIZE)
	{
		++key.x;
		chunkPos.x = 0;
	}
	if (chunkPos.z == Chunk::CHUNK_SIZE)
	{
		chunkPos.z = 0;
		++key.y;
	}
	if (_chunkMap.count(key) == 0)
	{
		res.y = -1;
		return;
	}
	glm::vec4	offset(0.0f);
	
	if (build)
		GetCursorBlockOffset(offset, chunkPos, objCoord, key);
	res = glm::vec4(glm::round(objCoord.x / 2.0f), glm::round(objCoord.y / 2.0f), glm::round(objCoord.z / 2.0f), offset.w);
	
	if (_chunkMap[key]->getBlockAt((int)glm::floor(chunkPos.y / (float)(Chunk::CHUNK_SIZE)),
								   (int)chunkPos.x, (int)chunkPos.y % Chunk::CHUNK_SIZE, (int)chunkPos.z, id))
		return;
	else
		res.y = -1;
		*/
}

void
vox::Core::GetCursorBlockOffset(glm::vec4& offset, glm::vec3& chunkPos, glm::vec3& objCoord, Vector3Int& key)
{
	glm::vec3	Center((chunkPos.x + key.x * Chunk::CHUNK_SIZE) * 2, (chunkPos.y + key.y * Chunk::CHUNK_SIZE) * 2, (chunkPos.z + key.z * Chunk::CHUNK_SIZE) * 2);

	glm::vec3	dirToCenter = objCoord - Center;

	float absX = glm::abs(dirToCenter.x);
	float absY = glm::abs(dirToCenter.y);
	float absZ = glm::abs(dirToCenter.z);

	if (absY > absX && absY > absZ)
	{
		offset.w = dirToCenter.y < 0 ? 0.0f : 1.0f;
		offset.y = dirToCenter.y < 0 ? -1.0f : 1.0f;
	}
	else if (absZ > absX && absZ > absY)
	{
		offset.w = dirToCenter.z < 0 ? 2.0f : 3.0f;
		offset.z = dirToCenter.z < 0 ? -1.0f : 1.0f;
	}
	else
	{
		offset.w = dirToCenter.x < 0 ? 4.0f : 5.0f;
		offset.x = dirToCenter.x < 0 ? -1.0f : 1.0f;
	}
	chunkPos += glm::vec3(offset.x, offset.y, offset.z);

	if (chunkPos.x == Chunk::CHUNK_SIZE)
	{
		++key.x;
		chunkPos.x = 0;
	}
	if (chunkPos.z == Chunk::CHUNK_SIZE)
	{
		chunkPos.z = 0;
		++key.y;
	}
}

void
vox::Core::errorCallback(int error, const char* desc)
{
	std::cout << "{CODE" << error << "} " << desc << std::endl;
}

void
vox::Core::handleKeypress(GLFWwindow* window, int theKey, int scancode, int theAction, int mods)
{
	(void)window;
	(void)scancode;
	(void)mods;
	Inputs*	inputs = Core::GetSingleton().GetInputs();
	
	if (theAction == GLFW_PRESS)
		inputs->HandleKeyDown(theKey);
	else if (theAction == GLFW_REPEAT)
		inputs->HandleKeyRepeat(theKey);
	else
		inputs->HandleKeyUp(theKey);
}

void
vox::Core::handleMouseMove(GLFWwindow* window, double mouseX, double mouseY)
{
	(void)window;

	Inputs*	inputs = Core::GetSingleton().GetInputs();

	inputs->HandleMouseMove((int)mouseX, (int)mouseY);
}

int
vox::Core::LoadEntityFromObj(id::Obj& obj, int askedId)
{
	if (askedId < 0)
		return -2;
	if (_entities.count(askedId) != 0)
		return -2;
	vox::Model3*	model = new (std::nothrow) vox::Model3();
	if (model == nullptr)
		return -1;
	if (!model->Init(&obj))
		return -1;
	try { _entities.insert(std::pair<int, Entity*>(askedId, model)); }
	catch (std::bad_alloc) { delete model; return -1; }
	return askedId;
}

void
vox::Core::GetEntityTransform(int id, glm::vec3& pos, glm::vec3& rot)
{
	std::map<int, Entity*>::iterator it;
	
	it = _entities.find(id);
	if (it != _entities.end())
	{
		pos = it->second->pos;
		rot = it->second->rot;
	}
}

void
vox::Core::SetEntityTransform(int id, const glm::vec3& pos, const glm::vec3& rot)
{
	std::map<int, Entity*>::iterator it;
	
	it = _entities.find(id);
	if (it != _entities.end())
	{
		it->second->pos = pos;
		it->second->rot = rot;
	}
}

bool
vox::Core::AddLoadedChunk(vox::Chunk* chunk)
{
	if (chunk == nullptr || !chunk->initGL())
	{
		delete chunk;
		return false;
	}
	
	chunk->bindMesh();
	Vector3Int	ChunkPos;
	chunk->getPosition(ChunkPos);
	UnLoadChunk(ChunkPos);
	try { _chunkMap.insert(std::pair<Vector3Int, Chunk*>(ChunkPos, chunk)); }
	catch (std::bad_alloc) { delete chunk; return false; }
	return true;
}

bool
vox::Core::UnLoadChunk(const Vector3Int& chunkPos)
{
	std::map<Vector3Int, Chunk*>::iterator it;

	it = _chunkMap.find(chunkPos);
	if (it == _chunkMap.end())
		return false;
	delete it->second;
	_chunkMap.erase(it);
	return true;
}

void
vox::Core::GetPosPlayer(glm::vec3& pos, glm::vec3& rot)
{
	pos.x = -(cam->getXPos() / 2);
	pos.y = cam->getYPos() / 2;
	pos.z = -(cam->getZPos() / 2);
	rot.x = cam->getXRot();
	rot.y = cam->getYRot() + 90.0f;
	rot.z = cam->getZRot();
}

void
vox::Core::UpdateBlockChange(int x, int y, int z, int id)
{
	Vector3Int	chunkKey((int)glm::floor((float)x / Chunk::CHUNK_SIZE), (int)glm::floor((float)y / Chunk::CHUNK_SIZE), (int)glm::floor((float)z / Chunk::CHUNK_SIZE));
	std::map<Vector3Int, Chunk*>::iterator it;

	it = _chunkMap.find(chunkKey);
	if (it == _chunkMap.end())
		return;

	Chunk* chunk = it->second;
	int relX = x - chunkKey.x * Chunk::CHUNK_SIZE;
	int relY = z - chunkKey.y * Chunk::CHUNK_SIZE;
	int relZ = z - chunkKey.z * Chunk::CHUNK_SIZE;
	chunk->UpdateBlockAt(relX, relY, relZ, id);
}

//--------------------------------------------------------------------------------------

float3	randsphere(const float3 &center, float radius)
{
	float3	sample;
	float	radiusSq = radius * radius;
	do
	{
		sample = float3(	randfp(-radius, radius),
							randfp(-radius, radius),
							randfp(-radius, radius));
	} while (dot(sample, sample) > radiusSq);
	return sample + center;
}

static void	_SetupParticle_TriggerLayer1(ParticleStream &stream, const float3 &location, u32 indexStart, u32 indexEnd)
{
	assert(indexStart < stream.m_Count);
	assert(indexEnd <= stream.m_Count);

	for (u32 i = indexStart; i < indexEnd; i++)
	{
		float	life01			= randfp(0,1);
		stream.m_Lives[i]		= life01 * life01 * life01 * 3.0f + 0.5f;	// non-linear probability in the [0.5, 3.5] range
		stream.m_Sizes[i]		= 0.075f;

		stream.m_Positions[i]	= location;
		stream.m_Velocities[i]	= randsphere(float3(0,2.0,0), 3.5f);
		stream.m_Colors[i]		= 0xFFFFFFFF;	// argb (white). color updater will take care of changing the color

		// 'internal' stuff:
		stream.m_LifeRatios[i]	= 0.0f;		// should always be zero (hint: updaters can default-initialize the streams they 'own'
		stream.m_PrevPositions[i] = stream.m_Positions[i];
	}
}

static void	_SetupParticle_RootLayer(ParticleStream &stream, const float3 &location, u32 indexStart, u32 indexEnd)
{
	assert(indexStart < stream.m_Count);
	assert(indexEnd <= stream.m_Count);

	for (u32 i = indexStart; i < indexEnd; i++)
	{
		stream.m_Lives[i]		= randfp(1.0f, 2.0f);
		stream.m_Sizes[i]		= randfp(0.3f, 0.75f);

		stream.m_Positions[i]	= float3(	randfp(-7.0f, 7.0f),
											0.0f,
											randfp(-0.2f, 0.2f)) + location;
		stream.m_Velocities[i]	= randsphere(float3(0,8,0), 2.0f);

		stream.m_Colors[i]		= 0xFFFFFFFF;	// argb (white)

		// 'internal' stuff:
		stream.m_LifeRatios[i]	= 0.0f;		// should always be zero (hint: updaters can default-initialize the streams they 'own'
		stream.m_PrevPositions[i] = stream.m_Positions[i];
	}
}

void	_TriggerLayer1(ParticleWorld *targetWorld, void *triggerData, const float3 &location)
{
	assert(targetWorld != nullptr);

	// fireworks pellets

	ParticleEffect	*fx = new ParticleEffect;
	assert(fx != nullptr);
	if (fx->AddEmitter(new SimpleParticleEmitter(	static_cast<ParticleLayer*>(triggerData),	// dst layer
																	randfp(200, 1500.0f),						// particles per second
																	0.0f,										// spawn duration (in seconds)
																	location/*,									// spawn position
																	&_SetupParticle_TriggerLayer1*/)))		// particle init callback
	{
		targetWorld->AddEffect(fx);
	}
}

ParticleWorld*
vox::Core::_CreateNewParticleWorld()
{
	ParticleWorld	*world = new ParticleWorld;
	if (world != nullptr)
	{
		// create the two layers for our two types of particles:
		SimpleParticleLayer	*layerTrigger = new SimpleParticleLayer(&_SetupParticle_TriggerLayer1);	// particle init callback

		if (!world->AddLayer(layerTrigger) ||
			!layerTrigger->AddUpdater(new ParticleUpdaterLife) ||
			!layerTrigger->AddUpdater(new ParticleUpdaterPhysics(float3(0,-3.0f,0))) ||
			!layerTrigger->AddUpdater(new ParticleUpdaterColor(0xFFFF7020, 0x00000000)) ||						// from red to black
			!layerTrigger->AddRenderer(new ParticleRendererBillboard("Textures\\BB_Glow_02b_64.dds"))
			)
		{
			delete world;
			return nullptr;
		}

		SimpleParticleLayer	*layer = new SimpleParticleLayer(&_SetupParticle_RootLayer);	// particle init callback

		if (!world->AddLayer(layer) ||
			!layer->AddUpdater(new ParticleUpdaterDeathTrigger(&_TriggerLayer1, layerTrigger)) ||
			!layer->AddUpdater(new ParticleUpdaterLife) ||
			!layer->AddUpdater(new ParticleUpdaterPhysics(float3(0,-4.0f,0))) ||
			!layer->AddUpdater(new ParticleUpdaterColor(0xFFFFFFFF, 0xFFFF1000)) ||						// from white to red
			!layer->AddRenderer(new ParticleRendererBillboard("Textures\\LightRays_02.dds"))
			)
		{
			delete world;
			return nullptr;
		}

		ParticleEffect	*fx = new ParticleEffect;
		assert(fx != nullptr);
		if (!world->AddEffect(fx) ||
			!fx->AddEmitter(new SimpleParticleEmitter(layer, 10.0f, 1000.0f, float3(0,-4,0))))
		{
			delete world;
			return nullptr;
		}
	}
	return world;
}
