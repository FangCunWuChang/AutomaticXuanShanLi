#include "PathDialog.h"
#include "ui_PathDialog.h"

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

PathDialog::PathDialog(QWidget* parent)
	: QDialog(parent)
{
	ui = new Ui::PathDialog();
	ui->setupUi(this);

	QString configFile = QCoreApplication::applicationDirPath() + "/config.json";
	QFile file(configFile);
	if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QJsonDocument jd = QJsonDocument::fromJson(file.readAll());
		file.close();
		if (jd.isObject()) {
			QJsonArray path = jd["Path"].toArray();

			for (int i = 0; i < path.size(); i++) {
				ui->pathlist->addItem(path.at(i).toString());
			}
		}
	}

	ui->pathlist->setCurrentRow(ui->pathlist->count() - 1);
}

PathDialog::~PathDialog()
{
	delete ui;
}

void PathDialog::on_buttonBox_accepted()
{
	if (ui->pathlist->count() == 0) {
		QMessageBox::warning(this, "出错", "远程目录不可为空！");
		return;
	}
	QString configFile = QCoreApplication::applicationDirPath() + "/config.json";
	QFile file(configFile);
	if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QJsonDocument jd = QJsonDocument::fromJson(file.readAll());
		file.close();

		if (jd.isObject()) {
			QJsonObject jo = jd.object();

			QJsonArray pa;
			for (int i = 0; i < ui->pathlist->count(); i++) {
				pa.append(ui->pathlist->item(i)->text());
			}

			jo.insert("Path", pa);
			jd.setObject(jo);

			QByteArray jdata = jd.toJson(QJsonDocument::Indented);
			QFile fileo(configFile);
			if (file.exists()) {
				file.remove();
			}
			if (fileo.open(QIODevice::WriteOnly | QIODevice::Text)) {
				fileo.write(jdata);
				fileo.close();
			}
			else {
				QMessageBox::warning(this, "出错", "无法写入配置文件！");
				return;
			}
		}
	}
	else {
		QMessageBox::warning(this, "出错", "无法打开配置文件！");
		return;
	}
	this->accept();
}

void PathDialog::on_buttonBox_rejected()
{
	this->reject();
}

void PathDialog::on_pathlist_currentRowChanged(int currentRow)
{
	if (ui->pathlist->count() > 0) {
		ui->remove->setEnabled(currentRow >= 0 && currentRow < ui->pathlist->count());
	}
	else {
		ui->remove->setEnabled(false);
	}
}

void PathDialog::on_add_clicked()
{
	QString path = QInputDialog::getText(
		this,
		"添加目录",
		"输入目录名称",
		QLineEdit::Normal,
		QString(),
		nullptr,
		Qt::Dialog | Qt::WindowCloseButtonHint
	);
	if (!path.isEmpty()) {
		path.replace(' ', '-', Qt::CaseSensitive);

		ui->pathlist->insertItem(ui->pathlist->currentRow() + 1, path);
		ui->pathlist->setCurrentRow(ui->pathlist->currentRow() + 1);
	}
}

void PathDialog::on_addmacro_clicked()
{
	QStringList macroList = { "","$ORDER","$CLASS","$NAME","$ID" };
	QString path = QInputDialog::getItem(
		this,
		"添加宏",
		"选择一个目录宏",
		macroList,
		0,
		false,
		nullptr,
		Qt::Dialog | Qt::WindowCloseButtonHint
	);
	if (!path.isEmpty()) {
		ui->pathlist->insertItem(ui->pathlist->currentRow() + 1, path);
		ui->pathlist->setCurrentRow(ui->pathlist->currentRow() + 1);
	}
}

void PathDialog::on_remove_clicked()
{
	delete ui->pathlist->takeItem(ui->pathlist->currentRow());
}
