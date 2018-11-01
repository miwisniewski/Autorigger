#pragma once

#include "stdafx.h"
#include "ui_AR_GUI.h"

class CWindow : public QMainWindow
{
	Q_OBJECT

public:
	CWindow(QWidget *parent = Q_NULLPTR);

private:
	Ui::CWindowClass ui;
};
