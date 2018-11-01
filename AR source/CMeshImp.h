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
