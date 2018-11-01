#pragma once

#include "stdafx.h"
#include "CPopupWindow.h"

class CMeshImp
{
public:
	CMeshImp(const char*, QOpenGLFunctions_3_3_Core*);
	~CMeshImp();
	bool importMesh();
	aiScene* getScene();
	void render();

private:

	void normalizeSize();
	void createVAO();
	void createScene(const aiScene*);

	aiScene *scene;
	const char* path;

	std::vector<unsigned int> vertexIndices;
	std::vector<glm::vec3> Vertices;
	std::vector<glm::vec2> UVs;
	std::vector<glm::vec3> Normals;

	uint m1_VAO;
	GLuint m_Buffers[3];
	QOpenGLFunctions_3_3_Core *gl;
};
