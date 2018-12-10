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
#include "CWidget.h"

CWidget::CWidget(QWidget *parent) : QOpenGLWidget(parent)
{
	mesh_import_button = new QPushButton("Import new mesh", this);
	mesh_import_button->setGeometry(QRect(QPoint(550, 10),	QSize(120, 25)));
	connect(mesh_import_button, SIGNAL(released()), this, SLOT(mesh_import_button_handle()));

	tex_import_button = new QPushButton("Import new texture", this);
	tex_import_button->setGeometry(QRect(QPoint(675, 10), QSize(120, 25)));
	connect(tex_import_button, SIGNAL(released()), this, SLOT(tex_import_button_handle()));

	rig_button = new QPushButton("Rig", this);
	rig_button->setGeometry(QRect(QPoint(550, 40), QSize(120, 25)));
	connect(rig_button, SIGNAL(released()), this, SLOT(rig_button_handle()));

	export_button = new QPushButton("Export", this);
	export_button->setGeometry(QRect(QPoint(675, 40), QSize(120, 25)));
	connect(export_button, SIGNAL(released()), this, SLOT(export_button_handle()));

	reset_button = new QPushButton("Reset", this);
	reset_button->setGeometry(QRect(QPoint(612, 70), QSize(120, 25)));
	connect(reset_button, SIGNAL(released()), this, SLOT(reset_button_handle()));

	stopAni_button = new QPushButton("Stop animation", this);
	stopAni_button->setGeometry(QRect(QPoint(550, 160), QSize(245, 25)));
	connect(stopAni_button, SIGNAL(released()), this, SLOT(stopAni_button_handle()));

	playAni_button = new QPushButton("Play animation", this);
	playAni_button->setGeometry(QRect(QPoint(550, 130), QSize(245, 25)));
	connect(playAni_button, SIGNAL(released()), this, SLOT(playAni_button_handle()));

	rotationAxis = QVector3D(0.0, 0.0, 1.0);
	hasBones = false;
	imported = false;
	playAni = false;
	deleteAnim = false;
	mesh = nullptr;
	anim = nullptr;
}

CWidget::~CWidget()
{
	makeCurrent();
	delete texture;
	delete mesh_shader;
	delete anim_shader;
	delete camera;
	if (deleteAnim)
		delete anim;
	doneCurrent();
}

//	Mesh import button handler
void CWidget::mesh_import_button_handle()
{
	//open file dialog and let user import only obj files, returns file name and path to BA then makes it readable by importer
	QByteArray BA = QFileDialog::getOpenFileName(this, tr("Import obj model"), "", tr("Wavefront obj file(*.obj)")).toLocal8Bit();
	const char* Imesh = BA.constData();

	//if user just closes file dialog it still returns some garbage
	if (strlen(Imesh) > 3)
	{
		playAni = false;
		hasBones = false;
		initMeshes(Imesh, false);
		imported = true;
	}
}

//	Texture import button handler
void CWidget::tex_import_button_handle()
{
	//no idea why the conversions are any different than in mesh import handler, probably caused bugs
	QByteArray BA = QFileDialog::getOpenFileName(this, tr("Import png texture"), "", tr("Portable Network Graphics (*.png)")).toUtf8().constData();
	const char* Itex = BA.constData();

	//checks for garbage
	if (strlen(Itex) > 3)
		initTextures(Itex, false);
}

// Mesh export button handler
void CWidget::export_button_handle()
{
	QByteArray BA = QFileDialog::getSaveFileName(this, tr("Export COLLADA model"), "", tr("COLLADA  file (*.dae)")).toUtf8();
	const char* path = BA.constData();

	//checks for garbage
	if (strlen(path) > 3)
	{
		if (!hasBones)
		{
			qDebug() << "cWidget: Attempting boneless export";
			CMeshExp meshExp;
			meshExp.initScene(scene, path);
			if (meshExp.exportDAE())
				CPopupWindow *w = new CPopupWindow("Boneless mesh exported successfully", nullptr, "Export status", this);
			else
				CPopupWindow *w = new CPopupWindow("Error exporting boneless mesh", nullptr, "Export status", this);
		}
		else
		{
			qDebug() << "cWidget: Attempting skeletal export";
			CMeshExp meshExp;
			meshExp.initScene(scene, path);
			if (meshExp.exportDAE())
				CPopupWindow *w = new CPopupWindow("Skeletal mesh exported successfully", nullptr, "Export status", this);
			else
				CPopupWindow *w = new CPopupWindow("Error exporting skeletal mesh", nullptr, "Export status", this);
		}
	}
}

//	Rig process starter button handler
void CWidget::rig_button_handle()
{
	
	if (imported)
	{
		CMeshRig rigged(scene);
		scene = rigged.getScene();
		hasBones = true;
		CPopupWindow *w = new CPopupWindow("Mesh rigged successfully", nullptr, "Rigging status", this);
	}
	else
		CPopupWindow *w = new CPopupWindow("No mesh imported", nullptr, "Rigging status", this);
}

//	Viewport scene reset button handler
void CWidget::reset_button_handle()
{
	imported = false;
	playAni = false;
	hasBones = false;
	initMeshes("Resources/Meshes/box.obj", true);
	initTextures("Resources/Textures/checker.png", true);
}

//	Animation stop button handler
void CWidget::stopAni_button_handle()
{
	playAni = false;
	//set pose to bind pose;
}

//	Animation start button handler
void CWidget::playAni_button_handle()
{
	if (hasBones)
	{
		QByteArray BA = QFileDialog::getOpenFileName(this, tr("Import FBX model"), "", tr("Filmbox FBX file(*.FBX)")).toLocal8Bit();
		const char* Imesh = BA.constData();

		if (strlen(Imesh) > 3)
		{
			anim = new CAnimation(Imesh, scene, gl);
			if (!anim->init())
			{
				CPopupWindow* w = new CPopupWindow("Error loading specified animation", nullptr, "Animation status", this);
				return;
			}
			playAni = true;
			deleteAnim = true;
		}

	}
	else
		CPopupWindow *w = new CPopupWindow("Mesh is not yet rigged", nullptr, "Animation status", this);
}

// Initialize render engine
void CWidget::initializeGL()
{
	QSurfaceFormat format;
	format.setVersion(3, 1);
	this->setFormat(format);

	gl = new QOpenGLFunctions_3_3_Core;
	gl->initializeOpenGLFunctions();
	gl->glClearColor(0.5, 0.5, 0.5, 1);

	if (!imported)
	{
		initTextures("Resources/Textures/checker.png", true);
		initMeshes("Resources/Meshes/box.obj", true);
	}

	initShaders();
	initCamera();

	gl->glEnable(GL_CULL_FACE);
	gl->glCullFace(GL_BACK);
	gl->glFrontFace(GL_CCW);
	gl->glEnable(GL_DEPTH_TEST);

	//timer required for mesh rotation by mouse
	timer.start(12, this);
}

// Parse and initialize shaders
void CWidget::initShaders()
{
	mesh_shader = new QOpenGLShaderProgram(this);

	CShaderParser* vertexShader = new CShaderParser("Resources/Shaders/mesh_vert.glsl");
	mesh_shader->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShader->getSource());
	delete vertexShader;

	CShaderParser* fragmentShader = new CShaderParser("Resources/Shaders/mesh_frag.glsl");
	mesh_shader->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShader->getSource());
	delete fragmentShader;

	mesh_shader->link();

	anim_shader = new QOpenGLShaderProgram(this);

	CShaderParser* _vertexShader = new CShaderParser("Resources/Shaders/anim_vert.glsl");
	anim_shader->addShaderFromSourceCode(QOpenGLShader::Vertex, _vertexShader->getSource());
	delete _vertexShader;

	CShaderParser* _fragmentShader = new CShaderParser("Resources/Shaders/anim_frag.glsl");
	anim_shader->addShaderFromSourceCode(QOpenGLShader::Fragment, _fragmentShader->getSource());
	delete _fragmentShader;

	anim_shader->link();
}

// Import and initialize textures
void CWidget::initTextures(const char* path, bool reset)
{
	texture = new QOpenGLTexture(QImage(path).mirrored());
	tex = texture->textureId();
	gl->glGenTextures(1, &tex);
	gl->glActiveTexture(GL_TEXTURE0);
	gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//this function is also called in scene reset so this bool prevents unnecessary info window popup after scene reset, yes it imports the checker board again if reset - potential memory leak
	if (!reset)
		CPopupWindow *w = new CPopupWindow("Texture imported Successfully", nullptr, "Import status", this);
}

// Import and initialize mesh
void CWidget::initMeshes(const char* path, bool reset)
{
	mesh = new CMeshImp(path, gl);
	if (mesh->importMesh())
	{
		//same thing as in initTextures() with the reset bool
		if(!reset)
			CPopupWindow *w = new CPopupWindow("Mesh imported Successfully", nullptr, "Import status", this);
		scene = mesh->getScene();
	}
	else
		CPopupWindow *w = new CPopupWindow("Error importing mesh", nullptr, "Import status", this);
}

// Initialize camera
void CWidget::initCamera()
{
	camera = new CCamera();
	camera->initialize();
}

//	checks for and saves mouse click position
void CWidget::mousePressEvent(QMouseEvent *e)
{
	mousePressPosition = QVector2D(e->localPos());
}

//	checks for mouse click release, gets variables required for mesh rotation
void CWidget::mouseReleaseEvent(QMouseEvent *e)
{
	//position difference between click and release
	QVector2D diff = QVector2D(e->localPos()) - mousePressPosition;
	//calculates rotation speed based on above
	qreal acc = diff.length() / 100.0;
	angularSpeed += acc;
}

//	updates scene
void CWidget::timerEvent(QTimerEvent *e)
{
	angularSpeed *= 0.975;
	if (angularSpeed < 0.01)
		angularSpeed = 0.0;
	else
		rotation = QQuaternion::fromAxisAndAngle(rotationAxis, angularSpeed) * rotation;

	update();
}

void CWidget::resizeGL(int w, int h)
{
	matrix.setToIdentity();
	matrix.perspective(60.0f, width() / height(), 0.1f, 1000.0f);
	matrix.lookAt(camera->getPos(), camera->getDir(), QVector3D(0.0f, 0.0f, 1.0f));
}

void CWidget::paintGL()
{
	gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	QMatrix4x4 mx;
	mx.translate(0.0, 0.0, -5.0);
	mx.rotate(rotation);

	gl->glUniform1i(gl->glGetUniformLocation(mesh_shader->programId(), "texture"), 0);
	gl->glBindTexture(GL_TEXTURE_2D, texture->textureId());

	
	if (playAni)
	{
		anim_shader->bind();
		anim_shader->setUniformValue("matrix", matrix * mx);
		anim->Render(anim_shader);
		anim_shader->release();
	}
	else
	{
		mesh_shader->bind();
		mesh_shader->setUniformValue("matrix", matrix * mx);
		if(mesh != nullptr)
			mesh->render();		
		mesh_shader->release();
	}
}