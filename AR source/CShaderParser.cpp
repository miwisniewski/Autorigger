#include "CShaderParser.h"

CShaderParser::CShaderParser(char path[])
{
	std::ifstream shaderFile;
	std::string * shaderCodeStr;
	shaderFile.open(path, std::ifstream::in);

	shaderCodeStr = new std::string(std::istreambuf_iterator<char>(shaderFile), std::istreambuf_iterator<char>());
	shaderFile.close();
	*shaderCodeStr += "\0";
	shaderCode = shaderCodeStr->c_str();
}

CShaderParser::~CShaderParser()
{
	
}

const char* CShaderParser::getSource()
{
	return shaderCode;
}