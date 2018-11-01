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
#include "CAnimation.h"

CAnimation::CAnimation(const char* inPath, aiScene* scene, QOpenGLFunctions_3_3_Core *inGL)
{
	m_pScene = scene;
	m_VAO = 0;
	m_NumBones = 0;
	path = inPath;
	RunningTime = 0.0f;
	gl = inGL;
}

bool CAnimation::init()
{
	const aiScene* anim = ani.ReadFile(path, 0);

	if (anim)
	{
		qDebug() << "CAnimation: Animation imported successfuly";

		m_pScene->mAnimations = new aiAnimation*[1];
		m_pScene->mAnimations[0] = new aiAnimation;
		m_pScene->mAnimations[0]->mDuration = anim->mAnimations[0]->mDuration;
		m_pScene->mAnimations[0]->mTicksPerSecond = anim->mAnimations[0]->mTicksPerSecond;
		m_pScene->mAnimations[0]->mChannels = new aiNodeAnim*[24];
		m_pScene->mAnimations[0]->mNumChannels = anim->mAnimations[0]->mNumChannels;

		for (int i = 0; i < m_pScene->mAnimations[0]->mNumChannels; i++)
		{
			m_pScene->mAnimations[0]->mChannels[i] = new aiNodeAnim;
			m_pScene->mAnimations[0]->mChannels[i]->mNodeName = anim->mAnimations[0]->mChannels[i]->mNodeName;
			m_pScene->mAnimations[0]->mChannels[i]->mNumPositionKeys = anim->mAnimations[0]->mChannels[i]->mNumPositionKeys;
			m_pScene->mAnimations[0]->mChannels[i]->mNumRotationKeys = anim->mAnimations[0]->mChannels[i]->mNumRotationKeys;
			m_pScene->mAnimations[0]->mChannels[i]->mNumScalingKeys = anim->mAnimations[0]->mChannels[i]->mNumScalingKeys;
			m_pScene->mAnimations[0]->mChannels[i]->mPositionKeys = anim->mAnimations[0]->mChannels[i]->mPositionKeys;
			m_pScene->mAnimations[0]->mChannels[i]->mPostState = anim->mAnimations[0]->mChannels[i]->mPostState;
			m_pScene->mAnimations[0]->mChannels[i]->mPreState = anim->mAnimations[0]->mChannels[i]->mPreState;
			m_pScene->mAnimations[0]->mChannels[i]->mRotationKeys = anim->mAnimations[0]->mChannels[i]->mRotationKeys;
			m_pScene->mAnimations[0]->mChannels[i]->mScalingKeys = anim->mAnimations[0]->mChannels[i]->mScalingKeys;
		}

		m_GlobalInverseTransform = m_pScene->mRootNode->mTransformation;
		m_GlobalInverseTransform.Inverse();
		InitFromScene();
		return true;
	}
	else
	{
		qDebug() << "CMeshImp: Error importing mesh";
		return false;
	}
}

void CAnimation::VertexBoneData::AddBoneData(uint BoneID, float Weight)
{
	for (int i = 0; i < 4; i++)
	{
		if (Weights[i] == 0.0) 
		{
			IDs[i] = BoneID;
			Weights[i] = Weight;
			return;
		}
	}
	assert(0);
}

bool CAnimation::InitFromScene()
{
	gl->glGenVertexArrays(1, &m_VAO);
	gl->glBindVertexArray(m_VAO);
	gl->glGenBuffers(5, m_Buffers);

	aiMesh *mesh = m_pScene->mMeshes[0];

	for (int i = 0; i < mesh->mNumVertices; i++)
	{
		aiVector3D &vertices = mesh->mVertices[i];
		glm::vec3 tmpV(vertices.x, vertices.y, vertices.z);
		Positions.push_back(tmpV);

		aiVector3D &uvs = mesh->mTextureCoords[0][i];
		glm::vec2 tmpU(uvs.x, uvs.y);
		TexCoords.push_back(tmpU);

		aiVector3D &normals = mesh->mNormals[i];
		glm::vec3 tmpN(normals.x, normals.y, normals.z);
		Normals.push_back(tmpN);
	}

	for (uint i = 0; i < mesh->mNumFaces; ++i)
	{
		const aiFace& face = mesh->mFaces[i];
		for (int j = 0; j < face.mNumIndices; j++)
			Indices.push_back(face.mIndices[j]);
	}
	NumIndices = Indices.size();
	LoadBones(m_pScene->mMeshes[0]);
	gl->glBindVertexArray(0);
	return true;
}

void CAnimation::LoadBones(const aiMesh* pMesh)
{
	Bones.resize(pMesh->mNumVertices);

	for (uint i = 0; i < pMesh->mNumBones; i++) 
	{
		uint BoneIndex = 0;
		std::string BoneName(pMesh->mBones[i]->mName.data);
		if (m_BoneMapping.find(BoneName) == m_BoneMapping.end()) 
		{
			BoneIndex = m_NumBones;
			m_NumBones++;

			BoneInfo bi;
			bi.BoneOffset = pMesh->mBones[i]->mOffsetMatrix;
			m_BoneInfo.push_back(bi);
			m_BoneMapping[BoneName] = BoneIndex;
		}
		else 
		{
			BoneIndex = m_BoneMapping[BoneName];
		}

		for (uint j = 0; j < pMesh->mBones[i]->mNumWeights; j++) 
		{
			float Weight = pMesh->mBones[i]->mWeights[j].mWeight;
			Bones[pMesh->mBones[i]->mWeights[j].mVertexId].AddBoneData(BoneIndex, Weight);
		}
	}
}

void CAnimation::Render(QOpenGLShaderProgram *shader)
{
	std::vector<aiMatrix4x4> Transforms;

	if (RunningTime > 1000)
		RunningTime = 0;

	RunningTime += 0.02;
	BoneTransform(RunningTime, Transforms);

	GLuint bones[24];
	for (int i = 0; i < 24; i++)
		bones[i] = shader->uniformLocation(("gBones[" + std::to_string(i) + "]").c_str());

	for (int i = 0; i < Transforms.size(); i++)
	{
		float* ptr = &Transforms[i].a1;
		gl->glUniformMatrix4fv(bones[i], 1, GL_TRUE, ptr);
	}

	gl->glBindVertexArray(m_VAO);

	gl->glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[1]);
	gl->glBufferData(GL_ARRAY_BUFFER, sizeof(Positions[0]) * Positions.size(), &Positions[0], GL_STATIC_DRAW);
	gl->glEnableVertexAttribArray(0);
	gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	gl->glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[3]);
	gl->glBufferData(GL_ARRAY_BUFFER, sizeof(TexCoords[0]) * TexCoords.size(), &TexCoords[0], GL_STATIC_DRAW);
	gl->glEnableVertexAttribArray(1);
	gl->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

	gl->glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[2]);
	gl->glBufferData(GL_ARRAY_BUFFER, sizeof(Normals[0]) * Normals.size(), &Normals[0], GL_STATIC_DRAW);
	gl->glEnableVertexAttribArray(2);
	gl->glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

	gl->glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[4]);
	gl->glBufferData(GL_ARRAY_BUFFER, sizeof(Bones[0]) * Bones.size(), &Bones[0], GL_STATIC_DRAW);
	gl->glEnableVertexAttribArray(3);
	gl->glVertexAttribIPointer(3, 4, GL_INT, sizeof(VertexBoneData), (const GLvoid*)0);

	gl->glEnableVertexAttribArray(4);
	gl->glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (const GLvoid*)16);

	gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[0]);
	gl->glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices[0]) * Indices.size(), &Indices[0], GL_STATIC_DRAW);
	gl->glDrawElements(GL_TRIANGLES, NumIndices, GL_UNSIGNED_INT, (void*)0);

	gl->glBindVertexArray(0);
}

uint CAnimation::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	for (uint i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++)
		if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime)
			return i;

	assert(0);
	return 0;
}

uint CAnimation::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumRotationKeys > 0);

	for (uint i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) 
		if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) 
			return i;

	assert(0);
	return 0;
}

uint CAnimation::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumScalingKeys > 0);

	for (uint i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) 
		if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) 
			return i;

	assert(0);
	return 0;
}

void CAnimation::CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumPositionKeys == 1) 
	{
		Out = pNodeAnim->mPositionKeys[0].mValue;
		return;
	}

	uint PositionIndex = FindPosition(AnimationTime, pNodeAnim);
	uint NextPositionIndex = (PositionIndex + 1);
	assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
	float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
	const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}

void CAnimation::CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumRotationKeys == 1) 
	{
		Out = pNodeAnim->mRotationKeys[0].mValue;
		return;
	}

	uint RotationIndex = FindRotation(AnimationTime, pNodeAnim);
	uint NextRotationIndex = (RotationIndex + 1);
	assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
	float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
	aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
	Out = Out.Normalize();
}

void CAnimation::CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumScalingKeys == 1) 
	{
		Out = pNodeAnim->mScalingKeys[0].mValue;
		return;
	}

	uint ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
	uint NextScalingIndex = (ScalingIndex + 1);
	assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
	float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
	const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}

void CAnimation::ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const aiMatrix4x4& ParentTransform)
{
	const aiAnimation* pAnimation = m_pScene->mAnimations[0];

	aiMatrix4x4 NodeTransformation(pNode->mTransformation);

	const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, pNode->mName.data);

	if (pNodeAnim) 
	{
		aiVector3D Scaling;
		CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);

		aiQuaternion RotationQ;
		CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);

		aiVector3D Translation;
		CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);

		NodeTransformation = aiMatrix4x4(Scaling, RotationQ, Translation);
	}

	if (m_BoneMapping.find(pNode->mName.data) != m_BoneMapping.end())
	{
		uint ii = m_BoneMapping[pNode->mName.data];
		m_BoneInfo[ii].FinalTransformation = m_GlobalInverseTransform * ParentTransform * NodeTransformation * m_BoneInfo[ii].BoneOffset;
	}

	for (uint i = 0; i < pNode->mNumChildren; i++) 
		ReadNodeHeirarchy(AnimationTime, pNode->mChildren[i], ParentTransform * NodeTransformation);
}

void CAnimation::BoneTransform(float TimeInSeconds, std::vector<aiMatrix4x4>& Transforms)
{
	aiMatrix4x4 Identity;

	float TicksPerSecond = (float)(m_pScene->mAnimations[0]->mTicksPerSecond != 0 ? m_pScene->mAnimations[0]->mTicksPerSecond : 25.0f);
	float TimeInTicks = TimeInSeconds * TicksPerSecond;
	float AnimationTime = fmod(TimeInTicks, (float)m_pScene->mAnimations[0]->mDuration);
	
	ReadNodeHeirarchy(AnimationTime, m_pScene->mRootNode, Identity);

	Transforms.resize(m_NumBones);

	for (uint i = 0; i < m_NumBones; i++) 
		Transforms[i] = m_BoneInfo[i].FinalTransformation;

}

const aiNodeAnim* CAnimation::FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName)
{
	for (uint i = 0; i < pAnimation->mNumChannels; i++) 
	{
		const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

		if (std::string(pNodeAnim->mNodeName.data) == NodeName) 
		{
			return pNodeAnim;
		}
	}

	return NULL;
}