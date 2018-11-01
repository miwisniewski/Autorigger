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