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

#include <stdafx.h>
#include "CMeshImp.h"
#include "CMeshExp.h"
#include "CMeshRig.h"
#include "CCamera.h"
#include "CShaderParser.h"
#include "CPopupWindow.h"
#include "CAnimation.h"

class CWidget : public QOpenGLWidget
{
	Q_OBJECT
public:
	explicit CWidget(QWidget *parent = 0);
	~CWidget();

	void initializeGL() override;
	void resizeGL(int w, int h) override;
	void paintGL() override;

private:
	void initShaders();
	void initTextures(const char* path, bool reset);
	void initMeshes(const char* path, bool reset);
	void initCamera();

	void mousePressEvent(QMouseEvent *e) override;
	void mouseReleaseEvent(QMouseEvent *e) override;
	void timerEvent(QTimerEvent *e) override;

	QVector2D mousePressPosition;
	QVector3D rotationAxis;
	qreal angularSpeed;
	QQuaternion rotation;
	QBasicTimer timer;

	aiScene* scene;
	aiScene* runScene;
	aiScene* jumpScene;
	CCamera *camera;
	QOpenGLTexture *texture;
	QOpenGLShaderProgram *mesh_shader;
	QOpenGLShaderProgram *anim_shader;

	QMatrix4x4 matrix;
	GLuint tex;
	CAnimation* anim;

	CMeshImp *mesh;
	bool hasBones;
	bool imported;
	bool playAni;
	bool deleteAnim;

	QPushButton *mesh_import_button;
	QPushButton *tex_import_button;
	QPushButton *export_button;
	QPushButton *rig_button;
	QPushButton *reset_button;
	QPushButton *stopAni_button;
	QPushButton *playAni_button;

	QOpenGLFunctions_3_3_Core *gl;

private slots:
	void mesh_import_button_handle();
	void tex_import_button_handle();
	void export_button_handle();
	void rig_button_handle();
	void reset_button_handle();
	void stopAni_button_handle();
	void playAni_button_handle();
};
