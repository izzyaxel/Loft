#pragma once

#include "../../def.hh"

#include <memory>
#include <commons/math/vec2.hh>
#include <commons/math/quaternion.hh>
#include <commons/math/mat4.hh>

struct Camera
{
	void update();
	
	///Get the view matrix this camera represents
	[[nodiscard]] mat4x4<float> getViewMatrix() const;
	
	[[nodiscard]] mat4x4<float> getOrthoProjectionMatrix() const;
	
	///Pan the camera relative to its current position
	void move(double x, double y);
	void move(vec3<double> const &amount);
	
	///Set the camera's position
	void setPos(double x, double y);
	void setPos(vec3<double> const &pos);
	
	void setViewSize(uint32_t width, uint32_t height);
	void setViewSize(vec2<uint32_t> const &newSize);
	
	vec2<double> *follow = nullptr; //Point to use for automatic tracking, nullptr to disable autotracking
	vec3<double> pos{}; //Z is depth for ortho cameras
	quat<double> rot{};
	vec2<uint32_t> viewSize{}; //The view dimensions into the scene
	float nearPlane = 0.0f, farPlane = 1.0f, fov = 45.0f;
};

struct CameraInfo
{
	inline CameraInfo() = default;
	
	inline explicit CameraInfo(Camera const &camera) : cameraPos(camera.pos), cameraRotation(camera.rot), fov(camera.fov), nearPlane(camera.nearPlane), farPlane(camera.farPlane) {}
	inline explicit CameraInfo(std::shared_ptr<Camera> const &camera) : cameraPos(camera->pos), cameraRotation(camera->rot), fov(camera->fov), nearPlane(camera->nearPlane), farPlane(camera->farPlane) {}
	inline explicit CameraInfo(std::unique_ptr<Camera> const &camera) : cameraPos(camera->pos), cameraRotation(camera->rot), fov(camera->fov), nearPlane(camera->nearPlane), farPlane(camera->farPlane) {}
	inline explicit CameraInfo(Camera const * const camera) : cameraPos(camera->pos), cameraRotation(camera->rot), fov(camera->fov), nearPlane(camera->nearPlane), farPlane(camera->farPlane) {}
	inline CameraInfo(vec3<double> const &cameraPos, quat<double> const &cameraRotation, float nearPlane, float farPlane, float fov) :
			cameraPos(cameraPos), cameraRotation(cameraRotation), fov(fov), nearPlane(nearPlane), farPlane(farPlane) {}
	
	vec3<double> cameraPos{};
	quat<double> cameraRotation{};
	float fov = 45.0f, nearPlane = 0.0f, farPlane = 1.0f;
};
