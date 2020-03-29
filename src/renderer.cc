#include "renderer.hh"
#include "assets.hh"
#include "util.hh"

#include <commons/math/quaternion.hh>
#include <SDL2/SDL_video.h>
#include <glad/glad.h>

Renderer::Renderer(UP<EventBus_t> const &eventBus, uint32_t contextWidth, uint32_t contextHeight)
{
	AR::init(); //Init prefab assets
	this->_contextWidth = contextWidth;
	this->_contextHeight = contextHeight;
	this->useBackBuffer();
	glScissor(0, 0, this->_contextWidth, this->_contextHeight);
	glViewport(0, 0, this->_contextWidth, this->_contextHeight);
	
	//Register event handlers
	eventBus->registerEventHandler<EventWindowSizeChanged>([this](uint32_t newWidth, uint32_t newHeight)
	{
		this->_contextWidth = newWidth;
		this->_contextHeight = newHeight;
		glScissor(0, 0, this->_contextWidth, this->_contextHeight);
		glViewport(0, 0, this->_contextWidth, this->_contextHeight);
	});
	eventBus->registerEventHandler<EventScreenshot>([this](std::string const &outputPath)
	{
		writeScreenshot(outputPath, this->_contextWidth, this->_contextHeight);
	});
}

Renderer::~Renderer()
{
	AR::terminateMeshes();
	AR::terminateTextures();
	AR::terminateShaders();
	AR::terminateASAFiles();
	AR::terminateAtlases();
}

void Renderer::clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::setClearColor(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
	this->_clearColor.fromRGBAf(r, g, b, a);
}

void Renderer::setSRGBClearColor(float r, float g, float b, float a)
{
	glClearColor(std::pow(r, 2.2f), std::pow(g, 2.2f), std::pow(b, 2.2f), a);
	this->_clearColor.fromRGBAf(std::pow(r, 2.2f), std::pow(g, 2.2f), std::pow(b, 2.2f), a);
}

void Renderer::setDepthTesting(bool depthTest)
{
	depthTest ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
}

void Renderer::setScissorTesting(bool scissorTest)
{
	scissorTest ? glEnable(GL_SCISSOR_TEST) : glDisable(GL_SCISSOR_TEST);
}

void Renderer::setBlend(bool blend)
{
	blend ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
}

void Renderer::setBlendMode(uint32_t src, uint32_t dst)
{
	glBlendFunc(src, dst);
}

void Renderer::setCullFace(bool culling)
{
	culling ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
}

uint32_t Renderer::getCurrentMonitorRefreshRate()
{
	SDL_DisplayMode dm;
	SDL_GetCurrentDisplayMode(0, &dm);
	return (uint32_t)dm.refresh_rate;
}

void Renderer::useBackBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::bindImage(uint32_t target, uint32_t const &handle, IO mode, CF format)
{
	glBindImageTexture(target, handle, 0, GL_FALSE, 0, (uint32_t)mode, (uint32_t)format);
}

void Renderer::startComputeShader(uint32_t contextWidth, uint32_t contextHeight)
{
	glDispatchCompute((uint32_t)(std::ceil((float)(contextWidth) / this->workSizeX)), (uint32_t)(std::ceil((float)(contextHeight) / this->workSizeY)), 1);
}

void Renderer::draw(DrawMode mode, size_t numElements)
{
	glDrawArrays((GLenum)mode, 0, (GLsizei)numElements);
}

void Renderer::drawRenderable(Renderable const &entry)
{
	quat<float> rotation;
	rotation.fromAxial(vec3<float>{entry.axis}, degToRad<float>(entry.rotation));
	vec3<float> roundedPos = vec3<float>{vec2<float>{entry.pos}, 0};
	roundedPos.round();
	this->_m = modelMatrix(roundedPos, rotation, vec3<float>(vec2<float>{entry.scale}, 1));
	this->_mvp = modelViewProjectionMatrix(this->_m, this->_v, this->_p);
	AR::getShader(entry.shaderID)->use();
	AR::getShader(entry.shaderID)->sendMat4f("mvp", &this->_mvp.data[0][0]);
	std::array<float, 12> quadVerts{0.5f, 0.5f, 0, -0.5f, 0.5f, 0, 0.5f, -0.5f, 0, -0.5f, -0.5f, 0};
	auto uvs = AR::getAtlas(entry.atlasID)->getUVsForTile(entry.name);
	std::array<float, 8> quadUVs{uvs.lowerRight.x(), uvs.lowerRight.y(), uvs.lowerLeft.x(), uvs.lowerLeft.y(),uvs.upperRight.x(), uvs.upperRight.y(), uvs.upperLeft.x(), uvs.upperLeft.y()};
	Mesh mesh(quadVerts.data(), quadVerts.size(), quadUVs.data(), quadUVs.size());
	mesh.use();
	this->draw(DrawMode::TRISTRIPS, mesh.numVerts);
}

void Renderer::render(RenderList renderList, Camera const &camera)
{
	this->clear();
	if(renderList.empty()) return;
	this->_v = camera.getViewMatrix();
	this->_p = camera.getOrthoProjectionMatrix();
	size_t curAtlas = renderList[0].atlasID;
	AR::getAtlas(curAtlas)->use(0);
	for(size_t i = 0; i < renderList.size(); i++)
	{
		auto const &entry = renderList[i];
		if(entry.atlasID != curAtlas)
		{
			curAtlas = entry.atlasID;
			AR::getAtlas(curAtlas)->use(0);
		}
		this->drawRenderable(entry);
	}
}
