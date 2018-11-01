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