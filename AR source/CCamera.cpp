#include "CCamera.h"

CCamera::CCamera()
{

}

CCamera::~CCamera()
{

}

void CCamera::initialize()
{
	pos = glm::vec3(-150, 200, 200);
	dir = glm::vec3(0, -70, 80);
}

QVector3D CCamera::getDir()
{
	QVector3D tmp(dir.x, dir.y, dir.z);
	return tmp;
}

QVector3D CCamera::getPos()
{
	QVector3D tmp(pos.x, pos.y, pos.z);
	return tmp;
}