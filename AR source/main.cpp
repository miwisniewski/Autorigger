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