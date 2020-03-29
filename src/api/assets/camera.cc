#include "camera.hh"

void Camera::update()
{
	if(this->follow)
	{
		this->pos.x() = this->follow->x() - (this->viewSize.x() / 2.0f);
		this->pos.y() = this->follow->y() - (this->viewSize.y() / 2.0f);
	}
}

mat4x4<float> Camera::getViewMatrix() const
{
	return viewMatrix<float>(quat<float>{this->rot}, vec3<float>{this->pos});
}

mat4x4<float> Camera::getOrthoProjectionMatrix() const
{
	return orthoProjectionMatrix<float>(0, this->viewSize.x(), this->viewSize.y(), 0, this->nearPlane, this->farPlane);
}

void Camera::move(double x, double y)
{
	this->pos.x() += x;
	this->pos.y() += y;
}

void Camera::move(vec3<double> const &amount)
{
	this->pos += amount;
}

void Camera::setPos(double x, double y)
{
	this->pos.x() = x;
	this->pos.y() = y;
}

void Camera::setPos(vec3<double> const &pos)
{
	this->pos = pos;
}

void Camera::setViewSize(uint32_t width, uint32_t height)
{
	this->viewSize = {width, height};
}

void Camera::setViewSize(vec2<uint32_t> const &newSize)
{
	this->viewSize = newSize;
}
