#pragma once

#include "stdafx.h"
#include "SContainers.h"



class CAnimation
{
public:

	CAnimation(const char*, aiScene*, QOpenGLFunctions_3_3_Core*);
	bool init();
	void Render(QOpenGLShaderProgram*);
	void BoneTransform(float TimeInSeconds, std::vector<aiMatrix4x4>& Transforms);

private:
	struct BoneInfo
	{
		aiMatrix4x4 BoneOffset;
		aiMatrix4x4 FinalTransformation;
	};

	struct VertexBoneData
	{
		uint IDs[4];
		float Weights[4];

		void AddBoneData(uint BoneID, float Weight);
	};

	void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	uint FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
	uint FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
	uint FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
	const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName);
	void ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const aiMatrix4x4& ParentTransform);
	bool InitFromScene();
	void LoadBones(const aiMesh* paiMesh);

	GLuint m_VAO;
	GLuint m_Buffers[5];

	Assimp::Importer ani;

	std::vector<glm::vec3> Positions;
	std::vector<glm::vec3> Normals;
	std::vector<glm::vec2> TexCoords;
	std::vector<VertexBoneData> Bones;
	std::vector<uint> Indices;
	std::vector<BoneInfo> m_BoneInfo;
	std::map<std::string, uint> m_BoneMapping;

	std::string path;
	aiScene* m_pScene;
	uint m_NumBones;
	uint NumIndices;
	aiMatrix4x4 m_GlobalInverseTransform;
	QOpenGLFunctions_3_3_Core *gl;
	float RunningTime;
};

