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
#include "SContainers.h"

class CMeshRig
{
public:
	CMeshRig(aiScene*);
	~CMeshRig();
	aiScene* getScene();

private:
	void rig();
	void allocateMeshData();
	void createBoneSystem();
	void createBoneMatrices();
	aiMatrix4x4 lookAt(glm::vec3, glm::vec3);
	aiMatrix4x4 worldToLocal(int);
	void createNodeTree();
	aiNode *createNewNode(std::string, aiMatrix4x4);
	aiNode *createNewNode(std::string, int);
	void createBone(int, std::string, glm::vec3, glm::vec3, bool, int, float);
	void createVertexWeights(bool);
	bool isSeen(glm::vec3, glm::vec3, float, int);
	void applyBones();

	aiScene* scene;
	std::vector<glm::vec3> bonePos;
	std::vector<vertex> vertices;
	std::vector<bone> bones;
};