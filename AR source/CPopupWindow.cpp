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