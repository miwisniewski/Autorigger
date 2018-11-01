#pragma once
#include "stdafx.h"

class CMeshExp
{
public:
	CMeshExp();
	~CMeshExp();
	void initScene(aiScene*, const char*);
	bool exportDAE();
private:
	aiScene *scene;
	const char* path;
};