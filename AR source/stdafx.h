#pragma once

#define M_PI   3.14159265358979323846264338327950288

#include <iostream>
#include <fstream>
#include <map>

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_core>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QPushButton>
#include <QFileDialog>
#include <QApplication>
#include <QMessageBox>
#include <QMainWindow>
#include <QBasicTimer>
#include <QMouseEvent>

#include <assimp/Exporter.hpp>
#include <assimp/IOSystem.hpp>
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/vector3.h>

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/geometric.hpp>

