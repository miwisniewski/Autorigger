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
#include "CMeshRig.h"
#include "CAnthropometry.h"

CMeshRig::CMeshRig(aiScene* in)
{
	scene = in;
	rig();
}

CMeshRig::~CMeshRig()
{

}

void CMeshRig::rig()
{
	qDebug() << "CMeshRig: Attempting rigging";
	allocateMeshData();
	createBoneSystem();
	createBoneMatrices();
	createNodeTree();
	createVertexWeights(false);
	applyBones();
}

//fill triangle and vertex struct
void CMeshRig::allocateMeshData()
{
	aiMesh *mesh = scene->mMeshes[0];

	for (uint i = 0; i < mesh->mNumFaces; ++i)
	{
		const aiFace &face = mesh->mFaces[i];

		for (int i = 0; i < face.mNumIndices; i++)
		{
			unsigned int idx = face.mIndices[i];
			glm::vec3 posi(mesh->mVertices[idx].x, mesh->mVertices[idx].y, mesh->mVertices[idx].z);
			glm::vec3 norm(mesh->mNormals[idx].x, mesh->mNormals[idx].y, mesh->mNormals[idx].z);

			vertex v;
			v.pos = posi;
			v.normal = norm;
			v.id = idx;
			for (int i = 0; i < 21; i++)
				v.weights[i] = 0;
			vertices.push_back(v);
		}
	}

	bones.resize(21);
	CAnthropometry a(vertices);
	bonePos = a.getBonePositions();
	qDebug() << "CMeshrig: Mesh allocated";
}

//Create and fill bone struct to bone vector
void CMeshRig::createBoneSystem()
{
	createBone(0, "head",				bonePos[1],		bonePos[0],		false,	 1, 0.20);
	createBone(1, "neck",				bonePos[2],		bonePos[1],		true,	 4, 0.09);
	createBone(2, "right_shoulder",		bonePos[2],		bonePos[15],	true,	 4, 0.12);
	createBone(3, "left_shoulder",		bonePos[2],		bonePos[14],	true,	 4, 0.12);
	createBone(4, "upper_spine",		bonePos[3],		bonePos[2],		true,	 7, 0.22);
	createBone(5, "right_arm",			bonePos[15],	bonePos[17],	true,	 2, 0.11);
	createBone(6, "left_arm",			bonePos[14],	bonePos[16],	true,	 3, 0.11);
	createBone(7, "lower_spine",		bonePos[4],		bonePos[3],		true,	10, 0.22);
	createBone(8, "left_forearm",		bonePos[16],	bonePos[18],	true,	 6, 0.07);
	createBone(9, "right_forearm",		bonePos[17],	bonePos[19],	true,	 5, 0.07);
	createBone(10, "hip",				bonePos[5],		bonePos[4],		true,	10, 0.20);
	createBone(11, "right_pelvis",		bonePos[5],		bonePos[7],		true,	10, 0.17);
	createBone(12, "left_pelvis",		bonePos[5],		bonePos[6],		true,	10, 0.17);
	createBone(13, "left_hand",			bonePos[18],	bonePos[20],	false,	 8, 0.08);
	createBone(14, "right_hand",		bonePos[19],	bonePos[21],	false,	 9, 0.08);
	createBone(15, "right_thigh",		bonePos[7],		bonePos[9],		true,	11, 0.13);
	createBone(16, "left_thigh",		bonePos[6],		bonePos[8],		true,	12, 0.13);
	createBone(17, "left_tibia",		bonePos[8],		bonePos[10],	true,	16, 0.12);
	createBone(18, "right_tibia",		bonePos[9],		bonePos[11],	true,	15, 0.12);
	createBone(19, "left_foot",			bonePos[10],	bonePos[12],	false,	17, 0.11);
	createBone(20, "right_foot",		bonePos[11],	bonePos[13],	false,	18, 0.11);

	for (int i = 0; i < vertices.size(); i++)
	{
		float first = 99990;
		float second = 9999;
		int f = 0, s = 0;
		std::vector<float> distances;

		for (int j = 0; j < bones.size(); j++)
			distances.push_back(glm::distance(bones[j].boneCenter, vertices[i].pos));

		for (int j = 0; j < bones.size(); j++)
		{
			if (first > distances[j])
			{
				second = first;
				s = f;
				first = distances[j];
				f = j;
			}
			else if (second > distances[j])
			{
				s = j;
				second = distances[j];
			}
		}

		vertices[i].secondClosestBone = s;
		vertices[i].closestBone = f;
	}
}

void CMeshRig::createNodeTree()
{
	aiNode *rootNode = new aiNode;
	rootNode->mName = "skeleton";
	rootNode->mTransformation.Scaling(aiVector3D(0.01), rootNode->mTransformation);

	aiNode **rootHub			= new aiNode*[2];
	aiNode **hipHub				= new aiNode*[3];
	aiNode **lowerspineHub		= new aiNode*[1];
	aiNode **upperspineHub		= new aiNode*[3];
	aiNode **neckHub			= new aiNode*[1];
	aiNode **rightshoulderHub	= new aiNode*[1];
	aiNode **rightarmHub		= new aiNode*[1];
	aiNode **rightforearmHub	= new aiNode*[1];
	aiNode **leftshoulderHub	= new aiNode*[1];
	aiNode **leftarmHub			= new aiNode*[1];
	aiNode **leftforearmHub		= new aiNode*[1];
	aiNode **leftbuttHub		= new aiNode*[1];
	aiNode **leftthighHub		= new aiNode*[1];
	aiNode **lefttibiaHub		= new aiNode*[1];
	aiNode **rightbuttHub		= new aiNode*[1];
	aiNode **rightthighHub		= new aiNode*[1];
	aiNode **righttibiaHub		= new aiNode*[1];

	rootHub[0]			= createNewNode("mesh",				1);
	rootHub[1]			= createNewNode("hip",				bones[10].localTransformMat);
	hipHub[0]			= createNewNode("lower_spine",		bones[7].localTransformMat);
	hipHub[1]			= createNewNode("left_pelvis",		bones[12].localTransformMat);
	hipHub[2]			= createNewNode("right_pelvis",		bones[11].localTransformMat);
	lowerspineHub[0]	= createNewNode("upper_spine",		bones[4].localTransformMat);
	upperspineHub[0]	= createNewNode("neck",				bones[1].localTransformMat);
	upperspineHub[1]	= createNewNode("right_shoulder",	bones[2].localTransformMat);
	upperspineHub[2]	= createNewNode("left_shoulder",	bones[3].localTransformMat);
	neckHub[0]			= createNewNode("head",				bones[0].localTransformMat);
	rightshoulderHub[0] = createNewNode("right_arm",		bones[5].localTransformMat);
	rightarmHub[0]		= createNewNode("right_forearm",	bones[9].localTransformMat);
	rightforearmHub[0]	= createNewNode("right_hand",		bones[14].localTransformMat);
	leftshoulderHub[0]	= createNewNode("left_arm",			bones[6].localTransformMat);
	leftarmHub[0]		= createNewNode("left_forearm",		bones[8].localTransformMat);
	leftforearmHub[0]	= createNewNode("left_hand",		bones[13].localTransformMat);
	leftbuttHub[0]		= createNewNode("left_thigh",		bones[16].localTransformMat);
	leftthighHub[0]		= createNewNode("left_tibia",		bones[17].localTransformMat);
	lefttibiaHub[0]		= createNewNode("left_foot",		bones[19].localTransformMat);
	rightbuttHub[0]		= createNewNode("right_thigh",		bones[15].localTransformMat);
	rightthighHub[0]	= createNewNode("right_tibia",		bones[18].localTransformMat);
	righttibiaHub[0]	= createNewNode("right_foot",		bones[20].localTransformMat);

	rightarmHub[0]		->addChildren(1, rightforearmHub);
	rightshoulderHub[0]	->addChildren(1, rightarmHub);
	leftarmHub[0]		->addChildren(1, leftforearmHub);
	leftshoulderHub[0]	->addChildren(1, leftarmHub);
	upperspineHub[0]	->addChildren(1, neckHub);
	upperspineHub[1]	->addChildren(1, rightshoulderHub);
	upperspineHub[2]	->addChildren(1, leftshoulderHub);
	lowerspineHub[0]	->addChildren(3, upperspineHub);
	leftthighHub[0]		->addChildren(1, lefttibiaHub);
	leftbuttHub[0]		->addChildren(1, leftthighHub);
	rightthighHub[0]	->addChildren(1, righttibiaHub);
	rightbuttHub[0]		->addChildren(1, rightthighHub);
	hipHub[0]			->addChildren(1, lowerspineHub);
	hipHub[1]			->addChildren(1, leftbuttHub);
	hipHub[2]			->addChildren(1, rightbuttHub);
	rootHub[1]			->addChildren(3, hipHub);
	rootNode			->addChildren(2, rootHub);

	scene->mRootNode = rootNode;
}

//Create single bone data
void CMeshRig::createBone(int num, std::string iName, glm::vec3 iJoint, glm::vec3 iEnd, bool half, int iParent, float infl)
{
	bones[num].name = iName;
	bones[num].joint = iJoint;
	bones[num].end = iEnd;
	bones[num].numWeights = 0;
	bones[num].parent = iParent;
	bones[num].influence = infl;
	if (half)
		bones[num].boneCenter = (iJoint + iEnd) * glm::vec3(0.5);
	else
		bones[num].boneCenter = iEnd;
}

//generate bone mTransform matrices for node tree
void CMeshRig::createBoneMatrices()
{
	for (int i = 0; i < bones.size(); i++)
	{
		bones[i].worldTransformMat = lookAt(bones[i].joint, bones[i].end);
	}

	for (int i = 0; i < bones.size(); i++)
	{
		bones[i].localTransformMat = worldToLocal(i);
	}
}

aiMatrix4x4 CMeshRig::worldToLocal(int boneID)
{
	if (boneID == bones[boneID].parent )
	{
		return bones[boneID].worldTransformMat;
	}
	else
	{
		aiMatrix4x4 parent = bones[bones[boneID].parent].worldTransformMat;
		aiMatrix4x4 child = bones[boneID].worldTransformMat;
		return parent.Inverse() * child;
	}
}

aiMatrix4x4 CMeshRig::lookAt(glm::vec3 pos, glm::vec3 tar)
{
	glm::vec3 v1 = glm::normalize(tar - pos);
	glm::vec3 v2 = glm::normalize(glm::vec3(tar.x + 50.0, tar.y, tar.z) - pos);
	glm::vec3 vs = glm::cross(v1, v2);
	glm::vec3 vn = glm::normalize(vs);
	float ca = glm::dot(v1, v2);
	glm::vec3 vt = vn * glm::vec3(1.0 - ca);
	glm::vec3 vv = glm::vec3(vt.x * vn.y, vt.y * vn.z, vt.z * vn.x);
	
	return  aiMatrix4x4(vt.x * vn.x + ca,	vv.x + vs.z,		vv.z - vs.y,		pos.x * 100,
						vv.x - vs.z,		vt.y * vn.y + ca,	vv.y + vs.x,		pos.y * 100,
						vv.z + vs.y,		vv.y - vs.x,		vt.z * vn.z + ca,	pos.z * 100,
						0,					0,					0,					1			);
}

void CMeshRig::applyBones()
{
	aiBone **bon = new aiBone*[21];

	for (int i = 0; i < 21; i++)
	{
		bon[i] = new aiBone;
		bon[i]->mName = bones[i].name;

		aiMatrix4x4 offset = bones[i].worldTransformMat;
		bon[i]->mOffsetMatrix = offset.Inverse();
		bon[i]->mNumWeights = bones[i].numWeights;
		aiVertexWeight *vWeights = new aiVertexWeight[bon[i]->mNumWeights];
		int it = 0;
		for (int j = 0; j < vertices.size(); j++)
		{
			if (vertices[j].weights[i] > 0)
			{
				aiVertexWeight vw;
				vw.mVertexId = vertices[j].id;
				vw.mWeight = vertices[j].weights[i];
				vWeights[it] = vw;
				it++;
			}
		}
		bon[i]->mWeights = vWeights;
	}

	scene->mMeshes[0]->mBones = bon;
	scene->mMeshes[0]->mNumBones = 21;
}

//Calculate vertex weights of the mesh and apply them
void CMeshRig::createVertexWeights(bool nearest)
{
	if (nearest)		//nearest weighting
	{
		for (int i = 0; i < vertices.size(); i++)
		{
			float shortest = 9999.0;
			int it = 0;
			for (int j = 0; j < bones.size(); j++)
			{
				float dist = glm::distance(bones[j].boneCenter, vertices[i].pos);
				if (dist < shortest)
				{
					shortest = dist;
					it = j;
				}
			}
			vertices[i].weights[it] = 1.0;
			bones[it].numWeights++;
		}
	}
	else		//bone glow
	{
		std::vector<float> l;				//bone length vector
		std::vector<int> vv;
		for (int i = 0; i < bones.size(); i++)
		{
			l.push_back(glm::distance(bones[i].end, bones[i].joint));
		}

		for (int i = 0; i < bones.size(); i++)
		{
			for (int j = 0; j < vertices.size(); j++)
			{
				float illumination = 0.0;
				float step = 0.01; //smaller step = better accuracy
				for (float lambda = 0; lambda < 1; lambda += step)
				{
					glm::vec3 boneDir = bones[i].end - bones[i].joint;
					glm::vec3 bonePoint = glm::vec3(lambda) * boneDir + bones[i].joint;
					glm::vec3 rayDir = bonePoint - vertices[j].pos;

					if (isSeen(bonePoint, vertices[j].pos, bones[i].influence, i))		// V defines visibility of the bone, binary
					{
						glm::vec3 L = glm::normalize(bonePoint - vertices[j].pos);		//dij = vj - blamb
						glm::vec3 N = vertices[j].normal;
						for (int nrm = 0; nrm < 2; nrm++)
						{
							N *= -1;

							float R = glm::max(glm::dot(L, N), 0.0f) / (glm::length(L) * glm::length(L));
							float T = glm::length(glm::cross(L, glm::normalize(boneDir)));
							illumination += R * T;
						}
					}
				}

				if (illumination * l[i] > 0)
				{
					float p = illumination * l[i];
					vertices[j].numBones++;
					vertices[j].weights[i] = illumination * l[i];
					bones[i].numWeights++;
				}
			}
		}
		
		//postprocess - find vertices without weights
		for (int i = 0; i < vertices.size(); i++)
		{
			if (vertices[i].numBones < 1)
			{
				vertices[i].numBones++;
				vertices[i].weights[vertices[i].closestBone] = 1.0f;
				bones[vertices[i].closestBone].numWeights++;
			}
		}
		

		//postprocess - normalize weights
		for (int i = 0; i < vertices.size(); i++)
		{
			float sum = 0;
			for (int j = 0; j < bones.size(); j++)
				sum += vertices[i].weights[j];

			for (int j = 0; j < bones.size(); j++)
				vertices[i].weights[j] /= sum;
		}
	}
}

bool CMeshRig::isSeen(glm::vec3 pos, glm::vec3 target, float influence, int boneID)
{
	if (glm::distance(pos, target) < influence)
	{
		if (boneID == 4 && (target.z > 1.70 || (target.y > 0.33 || target.y < -0.33)))
			return false;

		return true;
	}
		

	return false;
}

//Create root node of skeleton
aiNode* CMeshRig::createNewNode(std::string name, int numMeshes)
{
	aiNode* node = new aiNode;

	node->mName = name;
	node->mMeshes = new unsigned int[numMeshes];
	for (int i = 0; i < numMeshes; i++)
	{
		node->mMeshes[i] = 0;
	}
	node->mNumMeshes = numMeshes;

	return node;
}

//Create bone node of skeleton
aiNode* CMeshRig::createNewNode(std::string name, aiMatrix4x4 offsetMatrix)
{
	aiNode* node = new aiNode;

	node->mName = name;
	node->mNumMeshes = 0;
	node->mTransformation = offsetMatrix;

	return node;
}

//return aiScene
aiScene* CMeshRig::getScene()
{
	//scale mesh
	for (int i = 0; i < scene->mMeshes[0]->mNumVertices; i++)
	{
		aiVector3D tmp_vertices = scene->mMeshes[0]->mVertices[i];
		tmp_vertices.x = tmp_vertices.x * 100;
		tmp_vertices.y = tmp_vertices.y * 100;
		tmp_vertices.z = tmp_vertices.z * 100;
		scene->mMeshes[0]->mVertices[i] = tmp_vertices;
	}
	return scene;
}
