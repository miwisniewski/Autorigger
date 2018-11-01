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
#pragma once

#include "stdafx.h"

struct vertex
{
	int id;
	glm::vec3 pos;
	glm::vec3 normal;
	float weights[21];
	int numBones = 0;
	int closestBone;
	int secondClosestBone;
};

struct bone
{
	std::string name;
	glm::vec3 joint;
	glm::vec3 end;
	glm::vec3 boneCenter;
	aiMatrix4x4 localTransformMat;
	aiMatrix4x4 worldTransformMat;
	float influence;
	int numWeights;
	int parent;
};



struct boneInfo
{
	aiMatrix4x4 boneOffset;
	aiMatrix4x4 finalTransform;
};