#include "AutomaticXuanShanLi.h"
#include "ui_AutomaticXuanShanLi.h"

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

AutomaticXuanShanLi::AutomaticXuanShanLi(QWidget* parent)
	: QMainWindow(parent)
	, ui(new Ui::AutomaticXuanShanLiClass)
{
	ui->setupUi(this);

	this->configb = new QToolButton(this);
	this->aboutb = new QToolButton(this);
	this->aboutQtb = new QToolButton(this);
	this->sourceb = new QToolButton(this);

	this->tipl = new QLabel(this);

	this->configb->setText("设置");
	this->aboutb->setText("关于");
	this->aboutQtb->setText("关于Qt");
	this->sourceb->setText("开源页面");

	ui->mainToolBar->addWidget(this->configb);
	ui->mainToolBar->addWidget(this->aboutb);
	ui->mainToolBar->addWidget(this->aboutQtb);
	ui->mainToolBar->addWidget(this->sourceb);

	this->tipl->setText("就绪");

	ui->statusBar->addPermanentWidget(this->tipl);

	connect(this->configb, &QToolButton::clicked, this, &AutomaticXuanShanLi::on_config);
	connect(this->aboutb, &QToolButton::clicked, this, &AutomaticXuanShanLi::on_about);
	connect(this->aboutQtb, &QToolButton::clicked, this, &AutomaticXuanShanLi::on_aboutQt);
	connect(this->sourceb, &QToolButton::clicked, this, &AutomaticXuanShanLi::on_source);
}

AutomaticXuanShanLi::~AutomaticXuanShanLi()
{
	if (this->uThread != nullptr) {
		delete this->uThread;
	}
	delete this->sourceb;
	delete this->tipl;
	delete this->aboutQtb;
	delete this->aboutb;
	delete this->configb;
}


void AutomaticXuanShanLi::on_config()
{
	ConfigDialog cd(this);
	cd.setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
	cd.exec();
}

void AutomaticXuanShanLi::on_about()
{
	int ver = POCO_VERSION;
	char ch[4] = { 0 };
	ch[0] = (char)ver;
	ch[1] = *((char*)&ver + 1);
	ch[2] = *((char*)&ver + 2);
	ch[3] = *((char*)&ver + 3);
	std::string version = std::to_string((int)ch[3]) + "." + std::to_string((int)ch[2]) + "." + std::to_string((int)ch[1]) + "." + std::to_string((int)ch[0]);
	QString date(__DATE__);
	QString time(__TIME__);
	QString qver(qVersion());
	QString mess("合工大宣城校区程序设计基础作业提交工具.(C)2021 無常.保留所有权利.\nAutomaticXuanShanLi.(C)2021 WuChang.All rights reserved.\n编译时间:" + date + " " + time + "\nQt版本:" + qver + "\nPoco库版本:" + QString::fromStdString(version));
	QMessageBox::information(this, "关于此程序", mess);
}

void AutomaticXuanShanLi::on_aboutQt()
{
	QMessageBox::aboutQt(this, "关于Qt");
}

void AutomaticXuanShanLi::on_source()
{
	QDesktopServices::openUrl(QUrl(QLatin1String("https://github.com/FangCunWuChang/AutomaticXuanShanLi")));
}

void AutomaticXuanShanLi::on_add_clicked()
{
	auto fileCheckF = [&](QString fileName)->bool
	{
		for (int i = 0; i < ui->filelist->count(); i++) {
			if (ui->filelist->item(i)->text().right(ui->filelist->item(i)->text().length() - ui->filelist->item(i)->text().lastIndexOf('/') - 1) == fileName.right(fileName.length() - fileName.lastIndexOf('/') - 1)) {
				return false;
			}
		}
		return true;
	};

	auto outPutF = [](QString message)->QString
	{
		QDateTime dt = QDateTime::currentDateTime();
		QString dtLabel = "[" + dt.toString("yyyy-MM-dd hh:mm:ss") + "]";
		return dtLabel + message;
	};

	ui->output->clear();
	this->tipl->setText("添加文件");
	QStringList fileList = QFileDialog::getOpenFileNames(this, "添加文件", QDir::currentPath(), "作业允许的文件 (*.c *.h *.cpp *.hpp *.doc *.docx);;所有文件 (*)");
	if (fileList.size() > 0) {
		QFileInfo fileInfo(fileList.at(0));
		QDir::setCurrent(fileInfo.absolutePath());

		for (int i = 0; i < fileList.size(); i++) {
			QApplication::processEvents();
			if (fileCheckF(fileList.at(i))) {
				ui->filelist->addItem(fileList.at(i));
			}
			else {
				ui->output->appendPlainText(outPutF("列表中已存在同名文件：" + fileList.at(i)));
			}
		}
		this->tipl->setText("就绪");
		ui->output->appendPlainText(outPutF("添加完成"));
		QMessageBox::information(this, "添加文件", "添加完成");
	}
	ui->filelist->setCurrentRow(ui->filelist->count() - 1);
	this->tipl->setText("就绪");
}

void AutomaticXuanShanLi::on_remove_clicked()
{
	delete ui->filelist->takeItem(ui->filelist->currentRow());
}

void AutomaticXuanShanLi::on_remove_all_clicked()
{
	while (ui->filelist->count() > 0) {
		delete ui->filelist->takeItem(0);
	}
}

void AutomaticXuanShanLi::on_filelist_currentRowChanged(int currentRow)
{
	if (ui->filelist->count() > 0) {
		ui->remove_all->setEnabled(true);
		ui->remove->setEnabled(currentRow >= 0 && currentRow < ui->filelist->count());
	}
	else {
		ui->remove->setEnabled(false);
		ui->remove_all->setEnabled(false);
	}
}

void AutomaticXuanShanLi::on_notes_clicked()
{
	auto outPutF = [](QString message)->QString
	{
		QDateTime dt = QDateTime::currentDateTime();
		QString dtLabel = "[" + dt.toString("yyyy-MM-dd hh:mm:ss") + "]";
		return dtLabel + message;
	};

	ui->groupBox_3->setEnabled(false);
	ui->output->clear();
	this->tipl->setText("为代码文件添加头部注释");

	QString name, id;
	QString configFile = QCoreApplication::applicationDirPath() + "/config.json";
	QFile cfile(configFile);
	if (cfile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QJsonDocument jd = QJsonDocument::fromJson(cfile.readAll());
		cfile.close();
		if (jd.isObject()) {
			name = jd["Name"].toString();
			id = jd["ID"].toString();
		}
		else {
			ui->output->appendPlainText(outPutF("出错！配置文件格式有误！"));
			QMessageBox::warning(this, "出错", "配置文件格式有误！");
			this->tipl->setText("就绪");
			ui->groupBox_3->setEnabled(true);
			return;
		}
	}
	else {
		ui->output->appendPlainText(outPutF("出错！无法读取配置文件！"));
		QMessageBox::warning(this, "出错", "无法读取配置文件！");
		this->tipl->setText("就绪");
		ui->groupBox_3->setEnabled(true);
		return;
	}

	auto headF = [name, id](QString file)->QString
	{
		QString splitS("//*********************************************************\n");
		QString crS("// Added by AutomaticXuanShanLi.(C)2021 WuChang.All rights reserved.\n");
		QString fnS("// File name    :");
		QString atS("// Author       :");
		QString dtS("// Date         :");
		QString siS("// Student ID   :");
		QDate date = QDate::currentDate();
		QString dateS = date.toString("yyyy-MM-dd");

		QString headS = splitS + fnS + file + "\n" + atS + name + "\n" + dtS + dateS + "\n" + siS + id + "\n" + splitS + crS + "\n";
		return headS;
	};

	for (int i = 0; i < ui->filelist->count(); i++) {
		QApplication::processEvents();
		QString filepath = ui->filelist->item(i)->text();
		if (
			filepath.endsWith(".c", Qt::CaseInsensitive) ||
			filepath.endsWith(".cpp", Qt::CaseInsensitive) ||
			filepath.endsWith(".h", Qt::CaseInsensitive) ||
			filepath.endsWith(".hpp", Qt::CaseInsensitive)
			) {
			QString filename = filepath.right(filepath.length() - filepath.lastIndexOf("/") - 1);

			QFile file(filepath);
			if (file.exists()) {
				if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
					QByteArray fileData(file.readAll());
					file.close();

					if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
						file.write(this->insertAuto(fileData, headF(filename)));
						file.close();

						ui->output->appendPlainText(outPutF("添加头部注释：" + filepath));
					}
					else {
						ui->output->appendPlainText(outPutF("文件无法写入：" + filepath));
					}
				}
				else {
					ui->output->appendPlainText(outPutF("文件无法读取：" + filepath));
				}
			}
			else {
				ui->output->appendPlainText(outPutF("文件不存在：" + filepath));
			}
		}
	}
	ui->output->appendPlainText(outPutF("添加完成"));
	this->tipl->setText("就绪");
	QMessageBox::information(this, "为代码文件添加头部注释", "添加完成");
	ui->groupBox_3->setEnabled(true);
}

QByteArray AutomaticXuanShanLi::insertAuto(QByteArray data, QString note)
{
	auto outPutF = [](QString message)->QString
	{
		QDateTime dt = QDateTime::currentDateTime();
		QString dtLabel = "[" + dt.toString("yyyy-MM-dd hh:mm:ss") + "]";
		return dtLabel + message;
	};

	QTextCodec::ConverterState state;
	QTextCodec* codec = QTextCodec::codecForName("UTF-8");
	QString text = codec->toUnicode(data.constData(), data.size(), &state);
	if (state.invalidChars > 0) {
		text = QTextCodec::codecForName("GBK")->toUnicode(data);
		ui->output->appendPlainText(outPutF("编码:GBK --> UTF-8"));
	}
	else {
		text = data;
		ui->output->appendPlainText(outPutF("编码:UTF-8"));
	}

	return QString(note + text).toUtf8();
}

void AutomaticXuanShanLi::on_check_clicked()
{
	auto outPutF = [](QString message)->QString
	{
		QDateTime dt = QDateTime::currentDateTime();
		QString dtLabel = "[" + dt.toString("yyyy-MM-dd hh:mm:ss") + "]";
		return dtLabel + message;
	};

	ui->groupBox_3->setEnabled(false);
	ui->output->clear();
	this->tipl->setText("代码文件名检查");
	for (int i = 0; i < ui->filelist->count(); i++) {
		QApplication::processEvents();
		QString filepath = ui->filelist->item(i)->text();
		if (
			filepath.endsWith(".c", Qt::CaseInsensitive) ||
			filepath.endsWith(".cpp", Qt::CaseInsensitive) ||
			filepath.endsWith(".h", Qt::CaseInsensitive) ||
			filepath.endsWith(".hpp", Qt::CaseInsensitive)
			) {
			QString filename = filepath.right(filepath.length() - filepath.lastIndexOf("/") - 1);
			filename.remove(".c", Qt::CaseInsensitive);
			filename.remove(".cpp", Qt::CaseInsensitive);
			filename.remove(".h", Qt::CaseInsensitive);
			filename.remove(".hpp", Qt::CaseInsensitive);
			if (filename.size() == 4) {
				for (int j = 0; j < 4; j++) {
					if (filename.at(j) < '0' || filename.at(j) > '9') {
						ui->output->appendPlainText(outPutF("文件名存在数字以外的字符：" + filepath));
						break;
					}
				}
			}
			else {
				ui->output->appendPlainText(outPutF("文件名应为4位数字：" + filepath));
			}
		}
	}
	ui->output->appendPlainText(outPutF("检查完成"));
	this->tipl->setText("就绪");
	QMessageBox::information(this, "代码文件名检查", "检查完成");
	ui->groupBox_3->setEnabled(true);
}

void AutomaticXuanShanLi::on_upload_clicked()
{
	auto outPutF = [](QString message)->QString
	{
		QDateTime dt = QDateTime::currentDateTime();
		QString dtLabel = "[" + dt.toString("yyyy-MM-dd hh:mm:ss") + "]";
		return dtLabel + message;
	};

	ui->groupBox_3->setEnabled(false);
	ui->output->clear();
	this->tipl->setText("上传文件");

	QString address, user, password, code;
	QJsonArray pathArray;
	uint port = 21;
	QString name, id;

	QString configFile = QCoreApplication::applicationDirPath() + "/config.json";
	QFile cfile(configFile);
	if (cfile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QJsonDocument jd = QJsonDocument::fromJson(cfile.readAll());
		cfile.close();
		if (jd.isObject()) {
			address = jd["Server"].toString();
			port = jd["Port"].toInt();
			user = jd["User"].toString();
			password = jd["Password"].toString();

			name = jd["Name"].toString();
			id = jd["ID"].toString();

			pathArray = jd["Path"].toArray();

			code = jd["Code"].toString();
		}
		else {
			ui->output->appendPlainText(outPutF("出错！配置文件格式有误！"));
			QMessageBox::warning(this, "出错", "配置文件格式有误！");
			this->tipl->setText("就绪");
			ui->groupBox_3->setEnabled(true);
			return;
		}
	}
	else {
		ui->output->appendPlainText(outPutF("出错！无法读取配置文件！"));
		QMessageBox::warning(this, "出错", "无法读取配置文件！");
		this->tipl->setText("就绪");
		ui->groupBox_3->setEnabled(true);
		return;
	}

	QTextCodec::setCodecForLocale(QTextCodec::codecForName(code.toStdString().c_str()));

	if (this->uThread != nullptr) {
		disconnect(this->uThread, &UploadThread::exceptionMessage, this, &AutomaticXuanShanLi::on_FTPMessage);
		disconnect(this->uThread, &UploadThread::finished, this, &AutomaticXuanShanLi::on_FTPFinished);
		delete this->uThread;
		this->uThread = nullptr;
	}

	this->uThread = new UploadThread;

	connect(this->uThread, &UploadThread::exceptionMessage, this, &AutomaticXuanShanLi::on_FTPMessage);
	connect(this->uThread, &UploadThread::finished, this, &AutomaticXuanShanLi::on_FTPFinished);

	if (!this->uThread->FTP_Open(address, port)) {
		disconnect(this->uThread, &UploadThread::exceptionMessage, this, &AutomaticXuanShanLi::on_FTPMessage);
		disconnect(this->uThread, &UploadThread::finished, this, &AutomaticXuanShanLi::on_FTPFinished);
		delete this->uThread;
		this->uThread = nullptr;

		ui->output->appendPlainText(outPutF("出错！无法建立FTP连接！"));
		QMessageBox::warning(this, "出错", "无法建立FTP连接！");
		this->tipl->setText("就绪");
		ui->groupBox_3->setEnabled(true);
		return;
	}

	if (!this->uThread->FTP_Login(user, password)) {
		this->uThread->FTP_Close();
		disconnect(this->uThread, &UploadThread::exceptionMessage, this, &AutomaticXuanShanLi::on_FTPMessage);
		disconnect(this->uThread, &UploadThread::finished, this, &AutomaticXuanShanLi::on_FTPFinished);
		delete this->uThread;
		this->uThread = nullptr;

		ui->output->appendPlainText(outPutF("出错！无法登录FTP服务器！"));
		QMessageBox::warning(this, "出错", "无法登录FTP服务器！");
		this->tipl->setText("就绪");
		ui->groupBox_3->setEnabled(true);
		return;
	}

	for (int i = 0; i < pathArray.size(); i++) {
		QString path_str = pathArray.at(i).toString();
		path_str.replace(' ', '-', Qt::CaseSensitive);
		if (path_str == "$ORDER") {
			QStringList subPathL = this->uThread->FTP_GetDirectories();
			subPathL.prepend("");
			QString dirname = QInputDialog::getItem(
				this,
				"选择作业次数",
				"请选择作业次数",
				subPathL,
				0,
				false,
				nullptr,
				Qt::Dialog | Qt::WindowCloseButtonHint
			);
			if (dirname.isEmpty()) {
				this->uThread->FTP_Close();
				disconnect(this->uThread, &UploadThread::exceptionMessage, this, &AutomaticXuanShanLi::on_FTPMessage);
				disconnect(this->uThread, &UploadThread::finished, this, &AutomaticXuanShanLi::on_FTPFinished);
				delete this->uThread;
				this->uThread = nullptr;

				ui->output->appendPlainText(outPutF("出错！目录名不得为空！"));
				QMessageBox::warning(this, "出错", "目录名不得为空！");
				this->tipl->setText("就绪");
				ui->groupBox_3->setEnabled(true);
				return;
			}
			if (!this->uThread->FTP_SetDirectory(dirname)) {
				this->uThread->FTP_Close();
				disconnect(this->uThread, &UploadThread::exceptionMessage, this, &AutomaticXuanShanLi::on_FTPMessage);
				disconnect(this->uThread, &UploadThread::finished, this, &AutomaticXuanShanLi::on_FTPFinished);
				delete this->uThread;
				this->uThread = nullptr;

				ui->output->appendPlainText(outPutF("出错！无法访问目录：" + dirname));
				QMessageBox::warning(this, "出错", "无法访问目录：" + dirname);
				this->tipl->setText("就绪");
				ui->groupBox_3->setEnabled(true);
				return;
			}
		}
		else if (path_str == "$CLASS") {
			QStringList subPathL = this->uThread->FTP_GetDirectories();
			subPathL.prepend("");
			QString dirname = QInputDialog::getItem(
				this,
				"选择班级名称",
				"请选择班级名称",
				subPathL,
				0,
				false,
				nullptr,
				Qt::Dialog | Qt::WindowCloseButtonHint
			);
			if (dirname.isEmpty()) {
				this->uThread->FTP_Close();
				disconnect(this->uThread, &UploadThread::exceptionMessage, this, &AutomaticXuanShanLi::on_FTPMessage);
				disconnect(this->uThread, &UploadThread::finished, this, &AutomaticXuanShanLi::on_FTPFinished);
				delete this->uThread;
				this->uThread = nullptr;

				ui->output->appendPlainText(outPutF("出错！目录名不得为空！"));
				QMessageBox::warning(this, "出错", "目录名不得为空！");
				this->tipl->setText("就绪");
				ui->groupBox_3->setEnabled(true);
				return;
			}
			if (!this->uThread->FTP_SetDirectory(dirname)) {
				this->uThread->FTP_Close();
				disconnect(this->uThread, &UploadThread::exceptionMessage, this, &AutomaticXuanShanLi::on_FTPMessage);
				disconnect(this->uThread, &UploadThread::finished, this, &AutomaticXuanShanLi::on_FTPFinished);
				delete this->uThread;
				this->uThread = nullptr;

				ui->output->appendPlainText(outPutF("出错！无法访问目录：" + dirname));
				QMessageBox::warning(this, "出错", "无法访问目录：" + dirname);
				this->tipl->setText("就绪");
				ui->groupBox_3->setEnabled(true);
				return;
			}
		}
		else if (path_str == "$ID") {
			QStringList dirList = this->uThread->FTP_GetDirectories();
			QString subName = id;
			if (dirList.contains(id)) {
				subName += "M";
				if (dirList.contains(subName)) {
					for (int m = 1; true; m++) {
						if (!dirList.contains(subName + QString::number(m))) {
							subName += QString::number(m);
							break;
						}
					}
				}
				QMessageBox::Button result = QMessageBox::warning(
					this,
					"上传文件",
					QString("当前已存在名为" + id + "的目录\n是否要根据规则提交到补充目录" + subName + "中？"),
					QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
					QMessageBox::Cancel
				);
				if (result == QMessageBox::Yes) {
					if (!this->uThread->FTP_AddDirectory(subName)) {
						this->uThread->FTP_Close();
						disconnect(this->uThread, &UploadThread::exceptionMessage, this, &AutomaticXuanShanLi::on_FTPMessage);
						disconnect(this->uThread, &UploadThread::finished, this, &AutomaticXuanShanLi::on_FTPFinished);
						delete this->uThread;
						this->uThread = nullptr;

						ui->output->appendPlainText(outPutF("出错！无法创建目录：" + subName));
						QMessageBox::warning(this, "出错", "无法创建目录：" + subName);
						this->tipl->setText("就绪");
						ui->groupBox_3->setEnabled(true);
						return;
					}
				}
				else if (result == QMessageBox::No) {
					subName = id;
				}
				else
				{
					this->uThread->FTP_Close();
					disconnect(this->uThread, &UploadThread::exceptionMessage, this, &AutomaticXuanShanLi::on_FTPMessage);
					disconnect(this->uThread, &UploadThread::finished, this, &AutomaticXuanShanLi::on_FTPFinished);
					delete this->uThread;
					this->uThread = nullptr;

					ui->output->appendPlainText(outPutF("已取消上传"));
					QMessageBox::warning(this, "出错", "上传已取消！");
					this->tipl->setText("就绪");
					ui->groupBox_3->setEnabled(true);
					return;
				}
			}
			else {
				if (!this->uThread->FTP_AddDirectory(subName)) {
					this->uThread->FTP_Close();
					disconnect(this->uThread, &UploadThread::exceptionMessage, this, &AutomaticXuanShanLi::on_FTPMessage);
					disconnect(this->uThread, &UploadThread::finished, this, &AutomaticXuanShanLi::on_FTPFinished);
					delete this->uThread;
					this->uThread = nullptr;

					ui->output->appendPlainText(outPutF("出错！无法创建目录：" + subName));
					QMessageBox::warning(this, "出错", "无法创建目录：" + subName);
					this->tipl->setText("就绪");
					ui->groupBox_3->setEnabled(true);
					return;
				}
			}
			if (!this->uThread->FTP_SetDirectory(subName)) {
				this->uThread->FTP_Close();
				disconnect(this->uThread, &UploadThread::exceptionMessage, this, &AutomaticXuanShanLi::on_FTPMessage);
				disconnect(this->uThread, &UploadThread::finished, this, &AutomaticXuanShanLi::on_FTPFinished);
				delete this->uThread;
				this->uThread = nullptr;

				ui->output->appendPlainText(outPutF("出错！无法访问目录：" + subName));
				QMessageBox::warning(this, "出错", "无法访问目录：" + subName);
				this->tipl->setText("就绪");
				ui->groupBox_3->setEnabled(true);
				return;
			}
		}
		else if (path_str == "$NAME") {
			QStringList dirList = this->uThread->FTP_GetDirectories();
			QString subName = name.replace(' ', '-', Qt::CaseSensitive);
			if (dirList.contains(name)) {
				subName += "M";
				if (dirList.contains(subName)) {
					for (int m = 1; true; m++) {
						if (!dirList.contains(subName + QString::number(m))) {
							subName += QString::number(m);
							break;
						}
					}
				}
				QMessageBox::Button result = QMessageBox::warning(
					this,
					"上传文件",
					QString("当前已存在名为" + name + "的目录\n是否要根据规则提交到补充目录" + subName + "中？"),
					QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
					QMessageBox::Cancel
				);
				if (result == QMessageBox::Yes) {
					if (!this->uThread->FTP_AddDirectory(subName)) {
						this->uThread->FTP_Close();
						disconnect(this->uThread, &UploadThread::exceptionMessage, this, &AutomaticXuanShanLi::on_FTPMessage);
						disconnect(this->uThread, &UploadThread::finished, this, &AutomaticXuanShanLi::on_FTPFinished);
						delete this->uThread;
						this->uThread = nullptr;

						ui->output->appendPlainText(outPutF("出错！无法创建目录：" + subName));
						QMessageBox::warning(this, "出错", "无法创建目录：" + subName);
						this->tipl->setText("就绪");
						ui->groupBox_3->setEnabled(true);
						return;
					}
				}
				else if (result == QMessageBox::No) {
					subName = name;
				}
				else
				{
					this->uThread->FTP_Close();
					disconnect(this->uThread, &UploadThread::exceptionMessage, this, &AutomaticXuanShanLi::on_FTPMessage);
					disconnect(this->uThread, &UploadThread::finished, this, &AutomaticXuanShanLi::on_FTPFinished);
					delete this->uThread;
					this->uThread = nullptr;

					ui->output->appendPlainText(outPutF("已取消上传"));
					QMessageBox::warning(this, "出错", "上传已取消！");
					this->tipl->setText("就绪");
					ui->groupBox_3->setEnabled(true);
					return;
				}
			}
			else {
				if (!this->uThread->FTP_AddDirectory(subName)) {
					this->uThread->FTP_Close();
					disconnect(this->uThread, &UploadThread::exceptionMessage, this, &AutomaticXuanShanLi::on_FTPMessage);
					disconnect(this->uThread, &UploadThread::finished, this, &AutomaticXuanShanLi::on_FTPFinished);
					delete this->uThread;
					this->uThread = nullptr;

					ui->output->appendPlainText(outPutF("出错！无法创建目录：" + subName));
					QMessageBox::warning(this, "出错", "无法创建目录：" + subName);
					this->tipl->setText("就绪");
					ui->groupBox_3->setEnabled(true);
					return;
				}
			}
			if (!this->uThread->FTP_SetDirectory(subName)) {
				this->uThread->FTP_Close();
				disconnect(this->uThread, &UploadThread::exceptionMessage, this, &AutomaticXuanShanLi::on_FTPMessage);
				disconnect(this->uThread, &UploadThread::finished, this, &AutomaticXuanShanLi::on_FTPFinished);
				delete this->uThread;
				this->uThread = nullptr;

				ui->output->appendPlainText(outPutF("出错！无法访问目录：" + subName));
				QMessageBox::warning(this, "出错", "无法访问目录：" + subName);
				this->tipl->setText("就绪");
				ui->groupBox_3->setEnabled(true);
				return;
			}
		}
		else {
			if (!this->uThread->FTP_SetDirectory(path_str)) {
				this->uThread->FTP_Close();
				disconnect(this->uThread, &UploadThread::exceptionMessage, this, &AutomaticXuanShanLi::on_FTPMessage);
				disconnect(this->uThread, &UploadThread::finished, this, &AutomaticXuanShanLi::on_FTPFinished);
				delete this->uThread;
				this->uThread = nullptr;

				ui->output->appendPlainText(outPutF("出错！无法访问目录：" + path_str));
				QMessageBox::warning(this, "出错", "无法访问目录：" + path_str);
				this->tipl->setText("就绪");
				ui->groupBox_3->setEnabled(true);
				return;
			}
		}
	}

	QStringList filelist;
	for (int i = 0; i < ui->filelist->count(); i++) {
		filelist.append(ui->filelist->item(i)->text());
	}

	QStringList fileExists = this->uThread->FTP_GetFiles();

	QStringList fileUpList;

	int AllYesFlag = 0;
	for (int i = 0; i < filelist.size(); i++) {
		if (fileExists.contains(filelist.at(i).right(filelist.at(i).length() - filelist.at(i).lastIndexOf('/') - 1).replace(' ', '-', Qt::CaseSensitive))) {
			if (AllYesFlag > 0) {
				fileUpList.append(filelist.at(i));
				continue;
			}
			if (AllYesFlag < 0) {
				continue;
			}
			QMessageBox::Button result = QMessageBox::warning(
				this,
				"上传文件",
				QString("文件" + filelist.at(i).right(filelist.at(i).length() - filelist.at(i).lastIndexOf('/') - 1).replace(' ', '-', Qt::CaseSensitive) + "已经存在于服务器目录中\n是否上传该文件并覆盖服务器上的文件？"),
				QMessageBox::Yes | QMessageBox::No | QMessageBox::YesAll | QMessageBox::NoAll | QMessageBox::Cancel,
				QMessageBox::Cancel
			);
			if (result == QMessageBox::Yes) {
				fileUpList.append(filelist.at(i));
			}
			else if (result == QMessageBox::No) {
				//
			}
			else if (result == QMessageBox::YesAll) {
				fileUpList.append(filelist.at(i));
				AllYesFlag = 1;
			}
			else if (result == QMessageBox::NoAll) {
				AllYesFlag = -1;
			}
			else {
				this->uThread->FTP_Close();
				disconnect(this->uThread, &UploadThread::exceptionMessage, this, &AutomaticXuanShanLi::on_FTPMessage);
				disconnect(this->uThread, &UploadThread::finished, this, &AutomaticXuanShanLi::on_FTPFinished);
				delete this->uThread;
				this->uThread = nullptr;

				ui->output->appendPlainText(outPutF("已取消上传"));
				QMessageBox::warning(this, "出错", "上传已取消！");
				this->tipl->setText("就绪");
				ui->groupBox_3->setEnabled(true);
				return;
			}
		}
		else {
			fileUpList.append(filelist.at(i));
		}
	}

	this->uThread->setUploadFileList(fileUpList);
	this->uThread->start();
}

void AutomaticXuanShanLi::on_FTPFinished()
{
	auto outPutF = [](QString message)->QString
	{
		QDateTime dt = QDateTime::currentDateTime();
		QString dtLabel = "[" + dt.toString("yyyy-MM-dd hh:mm:ss") + "]";
		return dtLabel + message;
	};

	this->uThread->FTP_Close();
	disconnect(this->uThread, &UploadThread::exceptionMessage, this, &AutomaticXuanShanLi::on_FTPMessage);
	disconnect(this->uThread, &UploadThread::finished, this, &AutomaticXuanShanLi::on_FTPFinished);
	delete this->uThread;
	this->uThread = nullptr;

	ui->output->appendPlainText(outPutF("上传完成"));
	this->tipl->setText("就绪");
	QMessageBox::information(this, "上传文件", "上传完成");
	ui->groupBox_3->setEnabled(true);
}

void AutomaticXuanShanLi::on_FTPMessage(QString message)
{
	auto outPutF = [](QString message)->QString
	{
		QDateTime dt = QDateTime::currentDateTime();
		QString dtLabel = "[" + dt.toString("yyyy-MM-dd hh:mm:ss") + "]";
		return dtLabel + message;
	};

	ui->output->appendPlainText(outPutF(message));
}
