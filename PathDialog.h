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
#include <QInputDialog>
namespace Ui { class PathDialog; };

class PathDialog : public QDialog
{
	Q_OBJECT

public:
	PathDialog(QWidget* parent = Q_NULLPTR);
	~PathDialog();

private:
	Ui::PathDialog* ui;
private slots:

	void on_buttonBox_accepted();
	void on_buttonBox_rejected();

	void on_pathlist_currentRowChanged(int currentRow);

	void on_add_clicked();
	void on_addmacro_clicked();
	void on_remove_clicked();
};
