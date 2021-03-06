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
#include "CMeshExp.h"

CMeshExp::CMeshExp()
{
}

CMeshExp::~CMeshExp()
{
}

// Initialize aiScene
void CMeshExp::initScene(aiScene *inScene, const char* epath)
{
	scene = inScene;
	path = epath;
}

// Export rigged mesh
bool CMeshExp::exportDAE()
{
	Assimp::Exporter exporter;
	if (exporter.Export(scene, "collada", path) == AI_SUCCESS)
	{
		qDebug() << "CMeshExp: Successfully exported skeletal mesh";
		return true;
	}
	return false;
}