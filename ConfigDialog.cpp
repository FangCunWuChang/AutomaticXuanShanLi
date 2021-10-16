#include "ConfigDialog.h"
#include "ui_ConfigDialog.h"

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

ConfigDialog::ConfigDialog(QWidget* parent)
	: QDialog(parent)
{
	ui = new Ui::ConfigDialog();
	ui->setupUi(this);

	QString configFile = QCoreApplication::applicationDirPath() + "/config.json";
	QFile file(configFile);
	if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QJsonDocument jd = QJsonDocument::fromJson(file.readAll());
		file.close();
		if (jd.isObject()) {
			ui->name->setText(jd["Name"].toString());
			ui->id->setText(jd["ID"].toString());
			ui->address->setText(jd["Server"].toString());
			ui->port->setValue(jd["Port"].toInt());
			ui->user->setText(jd["User"].toString());
			ui->password->setText(jd["Password"].toString());
			ui->code->setCurrentText(jd["Code"].toString());
		}
	}
}

ConfigDialog::~ConfigDialog()
{
	delete ui;
}

void ConfigDialog::on_buttonBox_accepted()
{
	if (ui->name->text().isEmpty()) {
		QMessageBox::warning(this, "出错", "姓名不可为空！");
		return;
	}
	QString id = ui->id->text();
	if (id.size() != 10) {
		QMessageBox::warning(this, "出错", "学号应为10位数字！");
		return;
	}
	for (int i = 0; i < id.size(); i++) {
		if (id.at(i) < '0' || id.at(i) > '9') {
			QMessageBox::warning(this, "出错", "学号出现数字外的字符！");
			return;
		}
	}
	QString addrt = ui->address->text();
	if (addrt.isEmpty()) {
		QMessageBox::warning(this, "出错", "FTP服务器地址不可为空！");
		return;
	}
	QStringList addl = addrt.split(".", Qt::SkipEmptyParts);
	if (addl.size() != 4) {
		QMessageBox::warning(this, "出错", "FTP服务器地址格式有误！");
		return;
	}
	for (int i = 0; i < addl.size(); i++) {
		bool ok = true;
		uint temp = addl.at(i).toUInt(&ok, 10);
		if (!(ok && (temp >= 0) && (temp <= 255))) {
			QMessageBox::warning(this, "出错", "FTP服务器地址格式有误！");
			return;
		}
	}
	QString configFile = QCoreApplication::applicationDirPath() + "/config.json";
	QFile file(configFile);
	if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QJsonDocument jd = QJsonDocument::fromJson(file.readAll());
		file.close();

		if (jd.isObject()) {
			QJsonObject jo = jd.object();
			jo.insert("Name", ui->name->text());
			jo.insert("ID", ui->id->text());
			jo.insert("Server", ui->address->text());
			jo.insert("Port", ui->port->value());
			jo.insert("User", ui->user->text());
			jo.insert("Password", ui->password->text());
			jo.insert("Code", ui->code->currentText());
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

void ConfigDialog::on_buttonBox_rejected()
{
	this->reject();
}

void ConfigDialog::on_path_clicked()
{
	PathDialog pd(this);
	pd.setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
	pd.exec();
}
