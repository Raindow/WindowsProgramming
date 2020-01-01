#include "ImgProcessing.h"

ImgProcessing::ImgProcessing(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	whichPic = true;
	state = 1;
}

void ImgProcessing::on_action_Open_triggered()
{
	QString filename = QFileDialog::getOpenFileName(this, tr("选择图像"), "", tr("Images (*.png *.bmp *.jpg *.jfif)"));
	if (filename.isEmpty())
		return;
	else
	{
		QImage img;
		if (!(img.load(filename))) //加载图像
		{
			QMessageBox::information(this, tr("打开图像失败"), tr("打开图像失败!"));
			return;
		}
		if (srcs.size() < 2) {
			srcs.push_back(filename.toStdString());
		}else if(srcs.size() == 2) {
			srcs.pop_front();
		}

		if (whichPic) {
			ui.pic1->setPixmap(QPixmap::fromImage(img.scaled(ui.pic1->size())));
			whichPic = false;
		}
		else {
			ui.pic2->setPixmap(QPixmap::fromImage(img.scaled(ui.pic2->size())));
			whichPic = true;
		}
	}
}

void ImgProcessing::on_actionSave_triggered()
{
	QString runPath = QCoreApplication::applicationDirPath();       //获取exe路径
	QString hglpName = "photo";
	QString hglpPath = QString("%1/%2").arg(runPath).arg(hglpName);
	QScreen *screen = QGuiApplication::primaryScreen();
	screen->grabWindow(ui.pic3->winId()).save(QString("%1/34.jpg").arg(hglpPath));
}


void ImgProcessing::on_action_SaveAs_triggered()
{

	QString filename = QFileDialog::getSaveFileName(this, tr("Save Image"), "", tr("Images (*.png *.bmp *.jpg *.jfif)")); //选择路径
	QScreen *screen = QGuiApplication::primaryScreen();
	screen->grabWindow(ui.pic3->winId()).save(filename);
}


void ImgProcessing::on_action_Normal_triggered()
{
	state = 1;
	ui.action_SURF->setChecked(false);
	ui.action_ORB->setChecked(false);
	ui.action_OpencvStitch->setChecked(false);
}

void ImgProcessing::on_action_SURF_triggered()
{
	state = 3;
	ui.action_Normal->setChecked(false);
	ui.action_ORB->setChecked(false);
	ui.action_OpencvStitch->setChecked(false);
}

void ImgProcessing::on_action_ORB_triggered()
{
	state = 2;
	ui.action_Normal->setChecked(false);
	ui.action_SURF->setChecked(false);
	ui.action_OpencvStitch->setChecked(false);
}

void ImgProcessing::on_action_OpencvStitch_triggered()
{
	state = 4;
	ui.action_Normal->setChecked(false);
	ui.action_SURF->setChecked(false);
	ui.action_ORB->setChecked(false);
}



void ImgProcessing::on_pushButton_clicked()
{
	
	ImgProcessor* processor = new ImgProcessor();
	//ui.pic3->setPixmap(QPixmap::fromImage(processor->processing(state, ui.pic1->pixmap()->toImage(), ui.pic2->pixmap()->toImage()).scaled(ui.pic3->size())));
	ui.pic3->setPixmap(QPixmap::fromImage(processor->processing(state, srcs).scaled(ui.pic3->size())));
	ui.pushButton->setText("开       始       拼       合");
}
