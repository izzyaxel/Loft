#pragma once

#include <vector>
#include <string>
#include <cstdio>
#include <unordered_map>

struct Shader
{
	Shader() = delete;
	
	Shader(std::vector<uint8_t> const &vertShader, std::vector<uint8_t> const &fragShader);
	explicit Shader(std::vector<uint8_t> const &compShader);
	Shader(std::string const &vertShader, std::string const &fragShader);
	explicit Shader(std::string const &compShader);
	
	~Shader();
	
	//copy
	Shader(Shader &other);
	Shader& operator=(Shader other);
	
	//move
	Shader(Shader &&other);
	Shader& operator=(Shader &&other);
	
	void use();
	[[nodiscard]] int32_t getUniformHandle(std::string const &location);
	void sendFloat(std::string const &location, float val);
	void sendInt(std::string const &location, int32_t val);
	void sendUInt(std::string const &location, uint32_t val);
	void sendVec2f(std::string const &location, float* val);
	void sendVec3f(std::string const &location, float* val);
	void sendVec4f(std::string const &location, float* val);
	void sendMat3f(std::string const &location, float* val);
	void sendMat4f(std::string const &location, float* val);
	
	uint32_t handle = 0;
	std::unordered_map<std::string, int32_t> uniforms;
};
