#pragma once

#include "def.hh"
#include "api/render/texture.hh"
#include "api/render/shader.hh"
#include "api/render/mesh.hh"
#include "api/render/atlas.hh"
#include "api/assets/models.hh"

namespace AssetRepository
{
	extern uint64_t meshOrthoQuadLL, meshOrthoQuadC, meshOrthoQuadUL, meshOrthoQuadLR, meshOrthoQuadUR, meshFullscreenQuad;
	extern uint64_t shaderObject, shaderTransfer, shaderLine, shaderText;
	extern uint64_t shaderBlur3X, shaderBlur3Y, shaderBlur5X, shaderBlur5Y, shaderBlur7X, shaderBlur7Y, shaderBlur9X, shaderBlur9Y, shaderBlur11X, shaderBlur11Y, shaderBlur13X, shaderBlur13Y;
	extern uint64_t shaderBloom, shaderBloomComposite, shaderBloomSig, shaderDither, shaderTonemapACES, shaderTonemapFilmic, shaderTonemapSRGB, shaderTonemapUncharted2, shaderVignette;
	extern uint64_t textureFallback;
	
	void init();
	[[nodiscard]] uint64_t loadASA(std::string const &filePath);
	[[nodiscard]] uint64_t loadMeshFile(uint64_t asaID, std::string const &fileName);
	
	[[nodiscard]] uint64_t newTexture(uint64_t asaID, std::string const &fileName, bool srgb = false);
	[[nodiscard]] uint64_t newShader(uint64_t asaID, std::string const &compFileName);
	[[nodiscard]] uint64_t newShader(uint64_t asaID, std::string const &vertFileName, std::string const &fragFileName);
	[[nodiscard]] uint64_t newMesh(uint64_t meshID, std::string const &modelName);
	
	[[nodiscard]] uint64_t newTexture(std::vector<uint8_t> const &textureData, bool srgb = false);
	[[nodiscard]] uint64_t newTexture(uint32_t width, uint32_t height, ColorFormat format, InterpMode mode = InterpMode::Linear);
	[[nodiscard]] uint64_t newTexture(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
	[[nodiscard]] uint64_t newShader(std::vector<uint8_t> const &compShaderData);
	[[nodiscard]] uint64_t newShader(std::vector<uint8_t> const &vertShaderData, std::vector<uint8_t> const &fragShaderData);
	[[nodiscard]] uint64_t newShaderSrc(std::string const &vertSrc, std::string const &fragSrc);
	[[nodiscard]] uint64_t newShaderSrc(std::string const &compSrc);
	[[nodiscard]] uint64_t newMesh(std::vector<float> const &vertsData);
	[[nodiscard]] uint64_t newMesh(std::vector<float> const &vertsData, std::vector<float> const &uvsData);
	[[nodiscard]] uint64_t newMesh(std::vector<float> const &vertsData, std::vector<float> const &uvsData, std::vector<float> const &normalsData);
	[[nodiscard]] uint64_t newAtlas();
	
	void deleteASAFile(uint64_t id);
	void deleteTexture(uint64_t id);
	void deleteShader(uint64_t id);
	void deleteMesh(uint64_t id);
	void deleteAtlas(uint64_t id);
	
	[[nodiscard]] std::vector<uint8_t> getFileFromASA(uint64_t id, std::string const &filename);
	[[nodiscard]] MeshData getMesh(uint64_t meshID, std::string const &modelName);
	[[nodiscard]] UP<Texture>& getTexture(uint64_t id);
	[[nodiscard]] UP<Shader>& getShader(uint64_t id);
	[[nodiscard]] UP<Mesh>& getMesh(uint64_t id);
	[[nodiscard]] UP<Atlas>& getAtlas(uint64_t id);
	
	void terminateASAFiles();
	void terminateTextures();
	void terminateShaders();
	void terminateMeshes();
	void terminateAtlases();
}
namespace AR = AssetRepository;
