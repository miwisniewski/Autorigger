/*
System requirements:
x64 OS
GPU compatible with OpenGL 3.3

Used frameworks and libraries:
assimp 4.1.0
glm 0.9.8.5
QT 5.10
*/

#include "CWindow.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	app.setApplicationVersion("1.1.0.0");
	CWindow w;
	w.show();
	return app.exec();
}