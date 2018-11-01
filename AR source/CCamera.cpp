/*  Autorigger, tool that allows to create a skeleton and
	attach it to a three-dimensional model of a human body.
    Copyright (C) 2018  Michał Wiśniewski

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
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