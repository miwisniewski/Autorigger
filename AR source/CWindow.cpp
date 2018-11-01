#include "CWindow.h"

CWindow::CWindow(QWidget *parent) : QMainWindow(parent)
{
	ui.setupUi(this);
	
	this->setWindowTitle("Auto Rigger Utility");
}