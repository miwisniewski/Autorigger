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
#include "CPopupWindow.h"

CPopupWindow::CPopupWindow(QString message, QString detailedMessage, QString title, QWidget *parent) : QMessageBox(parent)
{
	this->resize(QSize(400, 150));
	if(detailedMessage != nullptr)
		this->setDetailedText(detailedMessage);
	
	if (message != nullptr)
		this->setText(message);

	if (title != nullptr)
		this->setWindowTitle(title);

	this->show();
}