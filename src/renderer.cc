#include "renderer.hh"
#include "gl.hh"
#include "assets.hh"
#include "util.hh"
#include "global.hh"

#include <algorithm>
#include <commons/math/quaternion.hh>
#include <SDL_video.h>
#include <glad/glad.h>

Renderer::Renderer(UP<EventBus_t> const &eventBus, uint32_t contextWidth, uint32_t contextHeight)
{
	AR::init(); //Init all prefab assets
	this->contextWidth = contextWidth;
	this->contextHeight = contextHeight;
	this->fboA = new Framebuffer(contextWidth, contextHeight, std::initializer_list<Attachment>{Attachment::Color, Attachment::Alpha}, "Ping"); //These FBOs are used to ping-pong the scene between for postprocessing
	this->fboB = new Framebuffer(contextWidth, contextHeight, std::initializer_list<Attachment>{Attachment::Color, Attachment::Alpha}, "Pong");
	this->scratch = new Framebuffer(contextWidth, contextHeight, std::initializer_list<Attachment>{Attachment::Color}, "Scratch");
	fboPool = new FramebufferPool(2, contextWidth, contextHeight);
	
	//Register event handlers
	eventBus->registerEventHandler<EventWindowSizeChanged>([this](uint32_t newWidth, uint32_t newHeight)
	                                                       {
		                                                       this->contextWidth = newWidth;
		                                                       this->contextHeight = newHeight;
		                                                       this->useBackBuffer();
		                                                       glViewport(0, 0, this->contextWidth, this->contextHeight);
		                                                       this->fboA->regenerate(this->contextWidth, this->contextHeight);
		                                                       this->fboB->regenerate(this->contextWidth, this->contextHeight);
	                                                       });
	eventBus->registerEventHandler<EventScreenshot>([this](std::string const &outputPath)
	                                                {
		                                                writeScreenshot(outputPath, this->contextWidth, this->contextHeight);
	                                                });
}

Renderer::~Renderer()
{
	delete this->fboA;
	delete this->fboB;
	delete this->scratch;
	delete fboPool;
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
	this->clearColor.fromRGBAf(r, g, b, a);
}

void Renderer::setSRGBClearColor(float r, float g, float b, float a)
{
	glClearColor(std::pow(r, 2.2f), std::pow(g, 2.2f), std::pow(b, 2.2f), a);
	this->clearColor.fromRGBAf(std::pow(r, 2.2f), std::pow(g, 2.2f), std::pow(b, 2.2f), a);
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

void Renderer::pingPong()
{
	this->curFBO.swap() ? this->fboA->use() : this->fboB->use();
	this->clear();
}

void Renderer::postProcessLayer(uint64_t layer, SP<LayerPostStack> const &layerPostStack)
{
	for(auto const &stage : layerPostStack->getPassesForLayer(layer))
	{
		if(stage->enabled)
		{
			this->pingPong();
			stage->process(this->curFBO.get() ? this->fboA : this->fboB, this->curFBO.get() ? this->fboB : this->fboA);
		}
	}
}

void Renderer::postProcessGlobal(SP<GlobalPostStack> const &globalPostStack)
{
	for(auto const &stage : globalPostStack->getPasses())
	{
		if(stage->enabled)
		{
			this->pingPong();
			stage->process(this->curFBO.get() ? this->fboA : this->fboB, this->curFBO.get() ? this->fboB : this->fboA);
		}
	}
}

void Renderer::drawToBackBuffer()
{
	AR::getMesh(AR::meshFullscreenQuad)->use();
	this->useBackBuffer();
	this->clear();
	AR::getShader(AR::shaderTransfer)->use();
	this->curFBO.get() ? this->fboA->bind(Attachment::Color, 0) : this->fboB->bind(Attachment::Color, 0);
	draw(DrawMode::TRISTRIPS, AR::getMesh(AR::meshFullscreenQuad)->numVerts);
}

void Renderer::drawToScratch()
{
	AR::getMesh(AR::meshFullscreenQuad)->use();
	this->scratch->use();
	AR::getShader(AR::shaderTransfer)->use();
	this->curFBO.get() ? this->fboA->bind(Attachment::Color, 0) : this->fboB->bind(Attachment::Color, 0);
	draw(DrawMode::TRISTRIPS, AR::getMesh(AR::meshFullscreenQuad)->numVerts);
}

void Renderer::scratchToPingPong()
{
	AR::getMesh(AR::meshFullscreenQuad)->use();
	this->pingPong();
	AR::getShader(AR::shaderTransfer)->use();
	scratch->bind(Attachment::Color, 0);
	draw(DrawMode::TRISTRIPS, AR::getMesh(AR::meshFullscreenQuad)->numVerts);
}

void Renderer::drawRenderable(Renderable const &entry) //TODO accept any type of geometry including lines, not just tristrips
{
	quat<float> rotation;
	rotation.fromAxial(vec3<float>{entry.axis}, degToRad<float>(entry.rotation));
	vec3<float> roundedPos = vec3<float>{vec2<float>{entry.pos}, 0};
	roundedPos.round();
	this->m = modelMatrix(roundedPos, rotation, vec3<float>(vec2<float>{entry.scale}, 1));
	this->mvp = modelViewProjectionMatrix(this->m, this->v, this->p);
	AR::getShader(entry.shaderID)->use();
	AR::getShader(entry.shaderID)->sendMat4f("mvp", &this->mvp.data[0][0]);
	std::array<float, 12> quadVerts{0.5f, 0.5f, 0, -0.5f, 0.5f, 0, 0.5f, -0.5f, 0, -0.5f, -0.5f, 0};
	auto uvs = AR::getAtlas(entry.atlasID)->getUVsForTile(entry.name);
	std::array<float, 8> quadUVs{uvs.lowerRight.x(), uvs.lowerRight.y(), uvs.lowerLeft.x(), uvs.lowerLeft.y(),uvs.upperRight.x(), uvs.upperRight.y(), uvs.upperLeft.x(), uvs.upperLeft.y()};
	Mesh mesh(quadVerts.data(), quadVerts.size(), quadUVs.data(), quadUVs.size());
	mesh.use();
	draw(DrawMode::TRISTRIPS, mesh.numVerts);
}

void Renderer::render(RenderList renderList, mat4x4<float> view, mat4x4<float> projection, SP<LayerPostStack> const &layerPostStack, SP<GlobalPostStack> const &globalPostStack)
{
	if(renderList.empty()) return;
	this->v = view;
	this->p = projection;
	size_t curAtlas = renderList[0].atlasID;
	AR::getAtlas(curAtlas)->use(0);
	if(!layerPostStack || layerPostStack->empty())
	{
		this->pingPong();
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
	else
	{
		this->scratch->use();
		this->clear();
		this->pingPong();
		bool bind = false;
		size_t prevLayer = renderList[0].layer;
		for(size_t i = 0; i < renderList.size(); i++)
		{
			auto const &entry = renderList[i];
			if(entry.atlasID != curAtlas)
			{
				bind = true;
				curAtlas = entry.atlasID;
			}
			if(i == 0)
			{
				if(bind) AR::getAtlas(curAtlas)->use(0);
				this->drawRenderable(entry);
			}
			else if(i == renderList.size() - 1)
			{
				if(entry.layer != prevLayer)
				{
					this->postProcessLayer(prevLayer, layerPostStack);
					this->drawToScratch();
					this->pingPong();
					AR::getAtlas(curAtlas)->use(0);
				}
				if(bind) AR::getAtlas(curAtlas)->use(0);
				this->drawRenderable(entry);
				this->postProcessLayer(entry.layer, layerPostStack);
				this->drawToScratch();
			}
			else
			{
				if(entry.layer != prevLayer)
				{
					this->postProcessLayer(prevLayer, layerPostStack);
					this->drawToScratch();
					this->pingPong();
					AR::getAtlas(curAtlas)->use(0);
				}
				if(bind) AR::getAtlas(curAtlas)->use(0);
				this->drawRenderable(entry);
			}
			prevLayer = entry.layer;
			bind = false;
		}
		this->scratchToPingPong();
	}
	if(globalPostStack && !globalPostStack->empty())
	{
		this->postProcessGlobal(globalPostStack);
	}
	this->drawToBackBuffer();
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
