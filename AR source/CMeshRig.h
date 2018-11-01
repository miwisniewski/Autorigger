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