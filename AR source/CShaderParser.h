#pragma once

#include "stdafx.h"

class CShaderParser
{
public:
	CShaderParser(char path[]);
	~CShaderParser();
	const char*  getSource();
private:
	const char* shaderCode;
};