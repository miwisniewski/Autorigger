#include "CMeshImp.h"

CMeshImp::CMeshImp(const char* inPath, QOpenGLFunctions_3_3_Core *inGL)
{
	path = inPath;
	m1_VAO = 0;
	gl = inGL;
	//gl->initializeOpenGLFunctions();

}

// Import, normalize, create VAO
bool CMeshImp::importMesh()
{
	Assimp::Importer importer;
	const aiScene* volatileScene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

	if (volatileScene)
	{
		qDebug() << "CMeshImp: AI mesh import successful";

		createScene(volatileScene);
		normalizeSize();
		createVAO();
		return true;
	}

	qDebug() << "CMeshImp: Error importing mesh";
	return false;
}

// Normalize mesh size to 200cm height
void CMeshImp::normalizeSize()
{
	aiMesh *mesh = scene->mMeshes[0];
	double scale = 0;
	float tmpMax = 0;

	for (int i = 0; i < mesh->mNumVertices; i++)
	{
		aiVector3D &vertices = mesh->mVertices[i];
		if (tmpMax < vertices.z)
		{
			tmpMax = vertices.z;
		}
	}
	scale = 2.0 / tmpMax;

	for (int i = 0; i < mesh->mNumVertices; i++)
	{
		aiVector3D &vertices = mesh->mVertices[i];
		glm::vec3 tmpV((vertices.x * scale), (vertices.y * scale), (vertices.z * scale));
		aiVector3D tmpVertices(tmpV.x, tmpV.y, tmpV.z);
		mesh->mVertices[i] = tmpVertices;
	}

	scene->mMeshes[0] = mesh;
	qDebug() << "CMeshImp: Normalized mesh size, new scale:" << scale;
}

// Create non const aiScene from AI import
void CMeshImp::createScene(const aiScene* volatileScene)
{
	aiMesh *mesh = volatileScene->mMeshes[0];
	scene = new aiScene;
	aiMesh *newMesh = new aiMesh;
	newMesh->mNumVertices = mesh->mNumVertices;
	newMesh->mVertices = new aiVector3D[mesh->mNumVertices];
	newMesh->mTextureCoords[0] = new aiVector3D[mesh->mNumVertices];
	newMesh->mNumUVComponents[0] = mesh->mNumVertices;
	newMesh->mNormals = new aiVector3D[mesh->mNumVertices];
	newMesh->mFaces = new aiFace[mesh->mNumFaces];
	newMesh->mNumFaces = mesh->mNumFaces;
	scene->mMeshes = new aiMesh*[1];
	scene->mNumMeshes = 1;
	scene->mRootNode = new aiNode;
	scene->mRootNode->mMeshes = new unsigned int[1];
	scene->mRootNode->mMeshes[0] = 0;
	scene->mRootNode->mNumMeshes = 1;
	scene->mMaterials = new aiMaterial*[1];
	scene->mMaterials[0] = new aiMaterial;
	scene->mNumMaterials = 1;
	newMesh->mMaterialIndex = 0;

	for (int i = 0; i < newMesh->mNumVertices; i++)
	{
		aiVector3D &vertices = mesh->mVertices[i];
		glm::vec3 tmpV(vertices.x, vertices.y, vertices.z);
		aiVector3D tmpVertices(tmpV.x, tmpV.y, tmpV.z);
		newMesh->mVertices[i] = tmpVertices;

		aiVector3D &uvs = mesh->mTextureCoords[0][i];
		glm::vec3 tmpU(uvs.x, uvs.y, uvs.z);
		aiVector3D tmpUVS(tmpU.x, tmpU.y, tmpU.z);
		newMesh->mTextureCoords[0][i] = tmpUVS;

		aiVector3D &normals = mesh->mNormals[i];
		glm::vec3 tmpN(normals.x, normals.y, normals.z);
		aiVector3D tmpNormals(tmpN.x, tmpN.y, tmpN.z);
		newMesh->mNormals[i] = tmpNormals;
	}


	for (uint i = 0; i < newMesh->mNumFaces; ++i)
	{
		const aiFace& face = mesh->mFaces[i];
		aiFace tmp(face);
		newMesh->mFaces[i] = tmp;
	}

	scene->mMeshes[0] = newMesh;
	qDebug() << "CMeshImp: Created new AI scene";
}

// Create vertex buffer object
void CMeshImp::createVAO()
{

	gl->glGenVertexArrays(1, &m1_VAO);
	gl->glBindVertexArray(m1_VAO);
	gl->glGenBuffers(3, m_Buffers);

	aiMesh *mesh = scene->mMeshes[0];
	
	for (int i = 0; i < mesh->mNumVertices; i++)
	{
		aiVector3D &vertices = mesh->mVertices[i];
		glm::vec3 tmpV(vertices.x * 100.0, vertices.y * 100.0, vertices.z * 100.0);
		Vertices.push_back(tmpV);

		aiVector3D &uvs = mesh->mTextureCoords[0][i];
		glm::vec2 tmpU(uvs.x, uvs.y);
		UVs.push_back(tmpU);

		aiVector3D &normals = mesh->mNormals[i];
		glm::vec3 tmpN(normals.x, normals.y, normals.z);
		Normals.push_back(tmpN);
	}

	for (uint i = 0; i < mesh->mNumFaces; ++i)
	{
		const aiFace& face = mesh->mFaces[i];
		for (int j = 0; j < face.mNumIndices; j++)
		{
			vertexIndices.push_back(face.mIndices[j]);
		}
	}
	
	gl->glBindVertexArray(0);
	qDebug() << "CMeshImp: Created VAO";
}


void CMeshImp::render()
{
	gl->glBindVertexArray(m1_VAO);

	gl->glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[1]);
	gl->glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * Vertices.size(), &Vertices[0], GL_STATIC_DRAW);
	gl->glEnableVertexAttribArray(0);
	gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	gl->glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[3]);
	gl->glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * UVs.size(), &UVs[0], GL_STATIC_DRAW);
	gl->glEnableVertexAttribArray(1);
	gl->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

	gl->glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[2]);
	gl->glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * Normals.size(), &Normals[0], GL_STATIC_DRAW);
	gl->glEnableVertexAttribArray(2);
	gl->glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

	gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[0]);
	gl->glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * vertexIndices.size(), &vertexIndices[0], GL_STATIC_DRAW);
	gl->glDrawElements(GL_TRIANGLES, vertexIndices.size(), GL_UNSIGNED_INT, (void*)0);

	gl->glBindVertexArray(0);
}

// Return imported aiScene
aiScene* CMeshImp::getScene()
{
	return scene;
}

CMeshImp::~CMeshImp()
{

}