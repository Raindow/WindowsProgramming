#pragma once
#pragma execution_character_set("utf-8")
#include <QtWidgets/QMainWindow>
#include "ui_ImgProcessing.h"
#include <qimage.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qpainter.h>
#include <qscreen.h>
#include <qcoreapplication.h>
#include <qguiapplication.h>
#include <deque>
#include <string>
#include "Imgprocessor.h"


class ImgProcessing : public QMainWindow
{
	Q_OBJECT

public:
	ImgProcessing(QWidget *parent = Q_NULLPTR);
	bool whichPic;
	int state;
	std::deque<std::string> srcs;


private slots:
	void on_action_Open_triggered();

	void on_action_Normal_triggered();

	void on_action_SURF_triggered();

	void on_action_ORB_triggered();

	void on_action_OpencvStitch_triggered();

	void on_action_SaveAs_triggered();

	void on_pushButton_clicked();

	void on_actionSave_triggered();

private:
	Ui::ImgProcessingClass ui;

};
