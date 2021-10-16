#pragma once

#include <QDialog>
#include <QMessageBox>
#include <QStringList>
#include <QFile>
#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QByteArray>
#include "PathDialog.h"

namespace Ui { class ConfigDialog; };

class ConfigDialog : public QDialog
{
	Q_OBJECT

public:
	ConfigDialog(QWidget* parent = Q_NULLPTR);
	~ConfigDialog();

private:
	Ui::ConfigDialog* ui;
private slots:
	void on_buttonBox_accepted();
	void on_buttonBox_rejected();
	void on_path_clicked();
};
