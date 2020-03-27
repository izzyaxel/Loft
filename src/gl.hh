#pragma once

#include "def.hh"

typedef void* (* GLloadproc)(const char *name);

enum struct GLVer
{
	GL33, GL45,
};

/// Access types for compute shader imnage binding
enum struct IO
{
	READ = 0x88B8, WRITE = 0x88B9, READWRITE = 0x88BA
};

/// Color format for compute shader image binding
enum struct CF
{
	R32F = 0x822E, RGB8 = 0x8051, RGBA8 = 0x8058, RGB16 = 0x8054, RGBA16 = 0x805B, RGB32I = 0x8D83, RGBA32I = 0x8D82,
	RGB32UI = 0x8D71, RGBA32UI = 0x8D70, RGB16F = 0x881B, RGBA16F = 0x881A, RGB32F = 0x8815, RGBA32F = 0x8814,
	DEPTH32F = 0x8CAC,
};

/// Mode to draw a VAO in
enum struct DrawMode
{
	TRIS = 0x0004, TRISTRIPS = 0x0005, TRIFANS = 0x0006,
	LINES = 0x0001, LINESTRIPS = 0x0003, LINELOOPS = 0x0002,
	POINTS = 0x0000,
};

void bindImage(uint32_t target, uint32_t const &handle, IO mode, CF format);

void startComputeShader(uint32_t contextWidth, uint32_t contextHeight);

void draw(DrawMode mode, size_t numElements);
