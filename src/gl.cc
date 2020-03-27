#include "gl.hh"

#include <glad/glad.h>
#include <cmath>

void bindImage(uint32_t target, uint32_t const &handle, IO mode, CF format)
{
	glBindImageTexture(target, handle, 0, GL_FALSE, 0, (uint32_t)mode, (uint32_t)format);
}

uint32_t workSizeX = 40, workSizeY = 20;

void startComputeShader(uint32_t contextWidth, uint32_t contextHeight)
{
	glDispatchCompute((uint32_t)(std::ceil((float)(contextWidth) / workSizeX)), (uint32_t)(std::ceil((float)(contextHeight) / workSizeY)), 1);
}

void draw(DrawMode mode, size_t numElements)
{
	glDrawArrays((GLenum)mode, 0, (GLsizei)numElements);
}

void clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
