#pragma once

#include "stdafx.h"

class CCamera
{
public:
	CCamera();
	~CCamera();
	void initialize();
	QVector3D getDir();
	QVector3D getPos();
private:
	glm::vec3 pos;
	glm::vec3 dir;
};