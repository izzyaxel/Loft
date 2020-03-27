#include "assets.hh"
#include "util.hh"
#include "global.hh"
#include "api/assets/asa.hh"
#include "api/assets/pngw.hh"

#include <vector>
#include <commons/misc.hh>

namespace AssetRepository
{
	uint64_t meshOrthoQuadLL, meshOrthoQuadC, meshOrthoQuadUL, meshOrthoQuadLR, meshOrthoQuadUR, meshFullscreenQuad;
	uint64_t shaderObject, shaderTransfer, shaderLine, shaderText;
	uint64_t shaderBlur3X, shaderBlur3Y, shaderBlur5X, shaderBlur5Y, shaderBlur7X, shaderBlur7Y, shaderBlur9X, shaderBlur9Y, shaderBlur11X, shaderBlur11Y, shaderBlur13X, shaderBlur13Y;
	uint64_t shaderBloom, shaderBloomComposite, shaderBloomSig, shaderDither, shaderTonemapACES, shaderTonemapFilmic, shaderTonemapSRGB, shaderTonemapUncharted2, shaderVignette;
	uint64_t textureFallback;
	UP<ASA> engineASA = nullptr;
	
	std::vector<UP<ASA>> asaFiles;
	std::vector<UP<MeshFile>> meshFiles;
	std::vector<UP<Texture>> textures;
	std::vector<UP<Shader>> shaders;
	std::vector<UP<Mesh>> meshes;
	std::vector<UP<Atlas>> atlases;
	
	UP<Texture> nTexture = nullptr;
	UP<Shader> nShader = nullptr;
	UP<Mesh> nMesh = nullptr;
	UP<Atlas> nAtlas = nullptr;
	std::vector<uint8_t> nFile{};
	MeshData nModel{};
	
	void init()
	{
		engineASA = ASA::open(getCWD() + "engine.asa");
		
		UP<MeshFile> quads = MeshFile::open(engineASA->read("quads.mesh"));
		if(!quads) throw std::runtime_error("Failed to load engine assets");
		MeshData c =  quads->read("centered");
		MeshData ll = quads->read("ll");
		MeshData lr = quads->read("lr");
		MeshData ul = quads->read("ul");
		MeshData ur = quads->read("ur");
		MeshData f =  quads->read("fullscreen");
		
		meshOrthoQuadC =            newMesh(c.vertElements, c.uvElements);
		meshOrthoQuadLL =           newMesh(ll.vertElements, ll.uvElements);
		meshOrthoQuadUL =           newMesh(ul.vertElements, ul.uvElements);
		meshOrthoQuadLR =           newMesh(lr.vertElements, lr.uvElements);
		meshOrthoQuadUR =           newMesh(ur.vertElements, ur.uvElements);
		meshFullscreenQuad =        newMesh(f.vertElements, f.uvElements);
		
		textureFallback =           newTexture(engineASA->read("fallback.png"));
		
		shaderObject =              newShader(engineASA->read("default.vert"), engineASA->read("default.frag"));
		shaderTransfer =            newShader(engineASA->read("transfer.vert"), engineASA->read("transfer.frag"));
		shaderLine =                newShader(engineASA->read("line.vert"), engineASA->read("line.frag"));
		shaderText =                newShader(engineASA->read("default.vert"), engineASA->read("text.frag"));
		
		shaderBloom =               newShader(engineASA->read("bloom.comp"));
		shaderBloomComposite =      newShader(engineASA->read("bloomComposite.comp"));
		shaderBloomSig =            newShader(engineASA->read("bloomSigmoid.comp"));
		shaderBlur3X =              newShader(engineASA->read("blur3X.comp"));
		shaderBlur3Y =              newShader(engineASA->read("blur3Y.comp"));
		shaderBlur5X =              newShader(engineASA->read("blur5X.comp"));
		shaderBlur5Y =              newShader(engineASA->read("blur5Y.comp"));
		shaderBlur7X =              newShader(engineASA->read("blur7X.comp"));
		shaderBlur7Y =              newShader(engineASA->read("blur7Y.comp"));
		shaderBlur9X =              newShader(engineASA->read("blur9X.comp"));
		shaderBlur9Y =              newShader(engineASA->read("blur9Y.comp"));
		shaderBlur11X =             newShader(engineASA->read("blur11X.comp"));
		shaderBlur11Y =             newShader(engineASA->read("blur11Y.comp"));
		shaderBlur13X =             newShader(engineASA->read("blur13X.comp"));
		shaderBlur13Y =             newShader(engineASA->read("blur13Y.comp"));
		shaderDither =              newShader(engineASA->read("dither.comp"));
		shaderTonemapACES =         newShader(engineASA->read("tonemapACES.comp"));
		shaderTonemapFilmic =       newShader(engineASA->read("tonemapFilmic.comp"));
		shaderTonemapSRGB =         newShader(engineASA->read("tonemapSRGB.comp"));
		shaderTonemapUncharted2 =   newShader(engineASA->read("tonemapUncharted2.comp"));
		shaderVignette =            newShader(engineASA->read("vignette.comp"));
	}
	
	uint64_t loadASA(std::string const &filePath)
	{
		uint64_t index = 0;
		for(uint64_t i = 0; i < asaFiles.size(); i++)
		{
			if(!asaFiles[i])
			{
				index = i;
				break;
			}
		}
		if(index == 0)
		{
			asaFiles.push_back(ASA::open(filePath));
			index = static_cast<uint64_t>(asaFiles.size() - 1);
		}
		else
		{
			asaFiles[index] = ASA::open(filePath);
		}
		return index;
	}
	
	uint64_t loadMeshFile(uint64_t asaID, std::string const &fileName)
	{
		std::vector<uint8_t> meshData = getFileFromASA(asaID, fileName);
		if(meshData.empty()) return 0;
		uint64_t index = 0;
		for(uint64_t i = 0; i < meshFiles.size(); i++)
		{
			if(!meshFiles[i])
			{
				index = i;
				break;
			}
		}
		if(index == 0)
		{
			meshFiles.push_back(MeshFile::open(meshData));
			index = static_cast<uint32_t>(meshFiles.size() - 1);
		}
		else
		{
			meshFiles[index] = MeshFile::open(meshData);
		}
		return index;
	}
	
	uint64_t newTexture(uint64_t asaID, std::string const &fileName, bool srgb)
	{
		std::vector<uint8_t> data = getFileFromASA(asaID, fileName);
		if(data.empty()) return 0;
		return newTexture(data, srgb);
	}
	
	uint64_t newShader(uint64_t asaID, std::string const &compFileName)
	{
		std::vector<uint8_t> compData = getFileFromASA(asaID, compFileName);
		if(compData.empty()) return 0;
		return newShader(compData);
	}
	
	uint64_t newShader(uint64_t asaID, std::string const &vertFileName, std::string const &fragFileName)
	{
		std::vector<uint8_t> vertData = getFileFromASA(asaID, vertFileName);
		std::vector<uint8_t> fragData = getFileFromASA(asaID, fragFileName);
		if(vertData.empty() || fragData.empty()) return 0;
		return newShader(vertData, fragData);
	}
	
	uint64_t newMesh(uint64_t meshID, std::string const &modelName) //TODO repo for model files
	{
		MeshData data = getMesh(meshID, modelName);
		if(data.numVertElements > 0)
		{
			if(data.numUVElements >0)
			{
				if(data.numNormalElements > 0)
				{
					return newMesh(data.vertElements, data.uvElements, data.normalElements);
				}
				return newMesh(data.vertElements, data.uvElements);
			}
			return newMesh(data.vertElements);
		}
		return 0;
	}
	
	uint64_t newTexture(std::vector<uint8_t> const &textureData, bool srgb)
	{
		uint64_t index = 0;
		for(uint64_t i = 0; i < textures.size(); i++)
		{
			if(!textures[i])
			{
				index = i;
				break;
			}
		}
		if(index == 0)
		{
			PNG tmp = decodePNG(textureData);
			textures.push_back(MU<Texture>(tmp.imageData.data(), tmp.width, tmp.height, tmp.colorFormat == 2 ? ColorFormat::RGB : ColorFormat::RGBA, InterpMode::Nearest, srgb));
			index = static_cast<uint32_t>(textures.size() - 1);
		}
		else
		{
			PNG tmp = decodePNG(textureData);
			textures[index] = MU<Texture>(tmp.imageData.data(), tmp.width, tmp.height, tmp.colorFormat == 2 ? ColorFormat::RGB : ColorFormat::RGBA, InterpMode::Nearest, srgb);
		}
		return index;
	}
	
	uint64_t newTexture(uint32_t width, uint32_t height, ColorFormat format, InterpMode mode)
	{
		uint64_t index = 0;
		for(uint64_t i = 0; i < textures.size(); i++)
		{
			if(!textures[i])
			{
				index = i;
				break;
			}
		}
		if(index == 0)
		{
			textures.push_back(MU<Texture>(width, height, format, mode));
			index = (uint32_t)(textures.size() - 1);
		}
		else textures[index] = MU<Texture>(width, height, format, mode);
		return index;
	}
	
	uint64_t newTexture(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
	{
		uint64_t index = 0;
		for(uint64_t i = 0; i < textures.size(); i++)
		{
			if(!textures[i])
			{
				index = i;
				break;
			}
		}
		if(index == 0)
		{
			textures.push_back(MU<Texture>(r, g, b, a));
			index = (uint32_t)(textures.size() - 1);
		}
		else textures[index] = MU<Texture>(r, g, b, a);
		return index;
	}
	
	uint64_t newShader(std::vector<uint8_t> const &compShaderData)
	{
		uint64_t index = 0;
		for(uint64_t i = 0; i < shaders.size(); i++)
		{
			if(!shaders[i])
			{
				index = i;
				break;
			}
		}
		if(index == 0)
		{
			shaders.push_back(MU<Shader>(compShaderData));
			index = (uint32_t)(shaders.size() - 1);
		}
		else shaders[index] = MU<Shader>(compShaderData);
		return index;
	}
	
	uint64_t newShader(std::vector<uint8_t> const &vertShaderData, std::vector<uint8_t> const &fragShaderData)
	{
		uint64_t index = 0;
		for(uint64_t i = 0; i < shaders.size(); i++)
		{
			if(!shaders[i])
			{
				index = i;
				break;
			}
		}
		if(index == 0)
		{
			shaders.push_back(MU<Shader>(vertShaderData, fragShaderData));
			index = (uint32_t)(shaders.size() - 1);
		}
		else shaders[index] = MU<Shader>(vertShaderData, fragShaderData);
		return index;
	}
	
	uint64_t newShaderSrc(std::string const &compSrc)
	{
		uint64_t index = 0;
		for(uint64_t i = 0; i < shaders.size(); i++)
		{
			if(!shaders[i])
			{
				index = i;
				break;
			}
		}
		if(index == 0)
		{
			shaders.push_back(MU<Shader>(compSrc));
			index = (uint32_t)(shaders.size() - 1);
		}
		else shaders[index] = MU<Shader>(compSrc);
		return index;
	}
	
	uint64_t newShaderSrc(std::string const &vertSrc, std::string const &fragSrc)
	{
		uint64_t index = 0;
		for(uint64_t i = 0; i < shaders.size(); i++)
		{
			if(!shaders[i])
			{
				index = i;
				break;
			}
		}
		if(index == 0)
		{
			shaders.push_back(MU<Shader>(vertSrc, fragSrc));
			index = (uint32_t)(shaders.size() - 1);
		}
		else shaders[index] = MU<Shader>(vertSrc, fragSrc);
		return index;
	}
	
	uint64_t newMesh(std::vector<float> const &verts)
	{
		uint64_t index = 0;
		for(uint64_t i = 0; i < meshes.size(); i++)
		{
			if(!meshes[i])
			{
				index = i;
				break;
			}
		}
		if(index == 0)
		{
			meshes.push_back(MU<Mesh>(verts));
			index = (uint32_t)(meshes.size() - 1);
		}
		else meshes[index] = MU<Mesh>(verts);
		return index;
	}
	
	uint64_t newMesh(std::vector<float> const &verts, std::vector<float> const &uvs)
	{
		uint64_t index = 0;
		for(uint64_t i = 0; i < meshes.size(); i++)
		{
			if(!meshes[i])
			{
				index = i;
				break;
			}
		}
		if(index == 0)
		{
			meshes.push_back(MU<Mesh>(verts, uvs));
			index = (uint32_t)(meshes.size() - 1);
		}
		else meshes[index] = MU<Mesh>(verts, uvs);
		return index;
	}
	
	uint64_t newMesh(std::vector<float> const &verts, std::vector<float> const &uvs, std::vector<float> const &normals)
	{
		uint64_t index = 0;
		for(uint64_t i = 0; i < meshes.size(); i++)
		{
			if(!meshes[i])
			{
				index = i;
				break;
			}
		}
		if(index == 0)
		{
			meshes.push_back(MU<Mesh>(verts, uvs, normals));
			index = (uint32_t)(meshes.size() - 1);
		}
		else meshes[index] = MU<Mesh>(verts, uvs, normals);
		return index;
	}
	
	uint64_t newAtlas()
	{
		uint64_t index = 0;
		for(uint64_t i = 0; i < atlases.size(); i++)
		{
			if(!atlases[i])
			{
				index = i;
				break;
			}
		}
		if(index == 0)
		{
			atlases.push_back(MU<Atlas>());
			index = (uint32_t)(atlases.size() - 1);
		}
		else atlases[index] = MU<Atlas>();
		return index;
	}
	
	void deleteASAFile(uint64_t id)
	{
		if(id >= asaFiles.size() - 1) logger << Sev::ERR << "Trying to delete invalid ASA ID: " << id << logger.endl();
		else if(!asaFiles[id]) logger << Sev::ERR << "Trying to delete an already deleted ASA file: " << id << logger.endl();
	}
	
	void deleteTexture(uint64_t id)
	{
		if(id >= textures.size() - 1) logger << Sev::ERR << "Trying to delete invalid texture ID: " << id << logger.endl();
		else if(!textures[id]) logger << Sev::ERR << "Trying to delete an already deleted texture: " << id << logger.endl();
		else textures[id].reset();
	}
	
	void deleteShader(uint64_t id)
	{
		if(id >= shaders.size() - 1) logger << Sev::ERR << "Trying to delete invalid shader ID: " << id << logger.endl();
		else if(!shaders[id]) logger << Sev::ERR << "Trying to delete an already deleted shader: " << id << logger.endl();
		else shaders[id].reset();
	}
	
	void deleteMesh(uint64_t id)
	{
		if(id >= meshes.size() - 1) logger << Sev::ERR << "Trying to delete invalid mesh ID: " << id << logger.endl();
		else if(!meshes[id]) logger << Sev::ERR << "Trying to delete an already deleted mesh: " << id << logger.endl();
		else meshes[id].reset();
	}
	
	void deleteAtlas(uint64_t id)
	{
		if(id >= atlases.size() - 1) logger << Sev::ERR << "Trying to delete invalid atlas ID: " << id << logger.endl();
		else if(!atlases[id]) logger << Sev::ERR << "Trying to delete an already deleted atlas: " << id << logger.endl();
		else atlases[id].reset();
	}
	
	std::vector<uint8_t> getFileFromASA(uint64_t id, std::string const &filename)
	{
		return (id <= asaFiles.size() - 1 && asaFiles[id]) ? asaFiles[id]->read(filename) : nFile;
	}
	
	MeshData getMesh(uint64_t meshID, std::string const &modelName) //TODO auto split the mesh files out into a modeldata repository on read?
	{
		return (meshID <= meshes.size() - 1 && meshes[meshID]) ? meshFiles[meshID]->read(modelName) : nModel;
	}
	
	UP<Texture>& getTexture(uint64_t id)
	{
		return (id <= textures.size() - 1 && textures[id]) ? textures[id] : nTexture;
	}
	
	UP<Shader>& getShader(uint64_t id)
	{
		return (id <= shaders.size() - 1 && shaders[id]) ? shaders[id] : nShader;
	}
	
	UP<Mesh>& getMesh(uint64_t id)
	{
		return (id <= meshes.size() - 1 && meshes[id]) ? meshes[id] : nMesh;
	}
	
	UP<Atlas>& getAtlas(uint64_t id)
	{
		return (id <= atlases.size() - 1 && atlases[id]) ? atlases[id] : nAtlas;
	}
	
	void terminateASAFiles()
	{
		asaFiles.clear();
	}
	
	void terminateTextures()
	{
		textures.clear();
	}
	
	void terminateShaders()
	{
		shaders.clear();
	}
	
	void terminateMeshes()
	{
		meshes.clear();
	}
	
	void terminateAtlases()
	{
		atlases.clear();
	}
}
