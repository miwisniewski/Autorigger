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

