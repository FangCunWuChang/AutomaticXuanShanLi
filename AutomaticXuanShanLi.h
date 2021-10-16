#pragma once

#include <QtWidgets/QMainWindow>
#include <QToolButton>
#include <QLabel>
#include <QMessageBox>
#include "ConfigDialog.h"
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QFileDialog>
#include <QStringList>
#include <QDateTime>
#include <QDate>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTextCodec>
#include "UploadThread.h"
#include <QInputDialog>
#include "Poco/Version.h"

namespace Ui { class AutomaticXuanShanLiClass; }

class AutomaticXuanShanLi : public QMainWindow
{
	Q_OBJECT

public:
	AutomaticXuanShanLi(QWidget* parent = Q_NULLPTR);
	~AutomaticXuanShanLi();

private:
	Ui::AutomaticXuanShanLiClass* ui;

	QToolButton* configb = nullptr;
	QToolButton* aboutb = nullptr;
	QToolButton* aboutQtb = nullptr;
	QLabel* tipl = nullptr;

	QByteArray insertAuto(QByteArray data, QString note);

	UploadThread* uThread = nullptr;

private slots:
	void on_config();
	void on_about();
	void on_aboutQt();

	void on_add_clicked();
	void on_remove_clicked();
	void on_remove_all_clicked();

	void on_filelist_currentRowChanged(int currentRow);

	void on_notes_clicked();
	void on_check_clicked();
	void on_upload_clicked();

	void on_FTPFinished();
	void on_FTPMessage(QString message);
};
