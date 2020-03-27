#pragma once

#include <initializer_list>
#include <vector>
#include <cstdint>
#include <array>

//TODO support index buffer
struct Mesh
{
	Mesh() = delete;
	
	~Mesh();
	
	//Base constructors, all others should be able to delegate to these
	Mesh(float const *verts, size_t vertsSize);
	Mesh(float const *verts, size_t vertsSize, float const *uvs, size_t uvsSize);
	Mesh(float const *verts, size_t vertsSize, float const *uvs, size_t uvsSize, float const *normals, size_t normalsSize);
	
	//float
	explicit Mesh(std::vector<float> const &verts);
	Mesh(std::vector<float> const &verts, std::vector<float> const &uvs);
	Mesh(std::vector<float> const &verts, std::vector<float> const &uvs, std::vector<float> const &normals);
	Mesh(std::initializer_list<float> const &verts);
	Mesh(std::initializer_list<float> const &verts, std::initializer_list<float> const &uvs);
	Mesh(std::initializer_list<float> const &verts, std::initializer_list<float> const &uvs, std::initializer_list<float> const &normals);
	
	template <size_t N> explicit Mesh(std::array<float, N> const &verts) : Mesh(verts.data(), verts.size()) {}
	template <size_t N> Mesh(std::array<float, N> const &verts, std::array<float, N> const &uvs) : Mesh(verts.data(), verts.size(), uvs.data(), uvs.size()) {}
	template <size_t N> Mesh(std::array<float, N> const &verts, std::array<float, N> const &uvs, std::array<float, N> const &normals) : Mesh(verts.data(), verts.size(), uvs.data(), uvs.size(), normals.data(), normals.size()) {}
	
	//uint8, raw data read out of files
	explicit Mesh(std::vector<uint8_t> const &verts);
	Mesh(std::vector<uint8_t> const &verts, std::vector<uint8_t> const &uvs);
	Mesh(std::vector<uint8_t> const &verts, std::vector<uint8_t> const &uvs, std::vector<uint8_t> const &normals);
	Mesh(std::initializer_list<uint8_t> const &verts);
	Mesh(std::initializer_list<uint8_t> const &verts, std::initializer_list<uint8_t> const &uvs);
	Mesh(std::initializer_list<uint8_t> const &verts, std::initializer_list<uint8_t> const &uvs, std::initializer_list<uint8_t> const &normals);
	
	template <size_t N> explicit Mesh(std::array<uint8_t, N> const &verts) : Mesh(reinterpret_cast<float const*>(verts.data()), verts.size() / 4) {}
	template <size_t N>  Mesh(std::array<uint8_t, N> const &verts, std::array<uint8_t, N> const &uvs) : Mesh(reinterpret_cast<float const*>(verts.data()), verts.size() / 4, reinterpret_cast<float const*>(uvs.data()), uvs.size() / 4) {}
	template <size_t N>  Mesh(std::array<uint8_t, N> const &verts, std::array<uint8_t, N> const &uvs, std::array<uint8_t, N> const &normals) : Mesh(reinterpret_cast<float const*>(verts.data()), verts.size() / 4, reinterpret_cast<float const*>(uvs.data()), uvs.size() / 4, reinterpret_cast<float const*>(normals.data()), normals.size() / 4) {}
	
	//copy
	Mesh(Mesh &other);
	Mesh& operator=(Mesh other);
	
	//move
	Mesh(Mesh &&other);
	Mesh& operator=(Mesh &&other);
	
	void use();
	
	uint32_t vao = 0, vboV = 0, vboU = 0, vboN = 0, vboI = 0;
	size_t numVerts = 0;
	bool hasVerts = false, hasUVs = false, hasNormals = false;

private:
	int32_t vertexStride = 3 * sizeof(float);
	int32_t uvStride = 2 * sizeof(float);
	int32_t normalStride = 3 * sizeof(float);
};
