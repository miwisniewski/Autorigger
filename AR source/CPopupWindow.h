#pragma once

#include "stdafx.h"

class CPopupWindow : public QMessageBox
{
	Q_OBJECT
public:
	CPopupWindow(QString, QString, QString, QWidget *parent = Q_NULLPTR);
};
