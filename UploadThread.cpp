#include "UploadThread.h"

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

UploadThread::UploadThread()
	: QThread()
{
	this->ftpsession = new Poco::Net::FTPClientSession;
}

UploadThread::~UploadThread()
{
	delete this->ftpsession;
}

bool UploadThread::FTP_Open(QString address, uint port)
{
	try {
		this->ftpsession->open(std::string(QTextCodec::codecForLocale()->fromUnicode(address).data()), port);
	}
	catch (Poco::Net::FTPException& e) {
		emit this->exceptionMessage(QTextCodec::codecForLocale()->toUnicode(QByteArray(e.displayText().c_str())));
	}
	return this->ftpsession->isOpen();
}

bool UploadThread::FTP_Login(QString user, QString password)
{
	if (user.isEmpty()) {
		user = "anonymous";
	}
	try {
		this->ftpsession->login(std::string(QTextCodec::codecForLocale()->fromUnicode(user).data()), std::string(QTextCodec::codecForLocale()->fromUnicode(password).data()));
		this->ftpsession->setPassive(false, false);
	}
	catch (Poco::Net::FTPException& e) {
		emit this->exceptionMessage(QTextCodec::codecForLocale()->toUnicode(QByteArray(e.displayText().c_str())));
	}
	return this->ftpsession->isLoggedIn();
}

bool UploadThread::FTP_SetDirectory(QString dir)
{
	if ((dir == ".") || dir == ".." || this->FTP_GetDirectories().contains(dir)) {
		try {
			this->ftpsession->setWorkingDirectory(std::string(QTextCodec::codecForLocale()->fromUnicode(dir).data()));
		}
		catch (Poco::Net::FTPException& e) {
			emit this->exceptionMessage(QTextCodec::codecForLocale()->toUnicode(QByteArray(e.displayText().c_str())));
			return false;
		}
		return true;
	}
	else {
		return false;
	}
}

bool UploadThread::FTP_AddDirectory(QString dir)
{
	if ((dir == ".") || dir == ".." || this->FTP_GetDirectories().contains(dir)) {
		return false;
	}
	else {
		try {
			this->ftpsession->createDirectory(std::string(QTextCodec::codecForLocale()->fromUnicode(dir).data()));
		}
		catch (Poco::Net::FTPException& e) {
			emit this->exceptionMessage(QTextCodec::codecForLocale()->toUnicode(QByteArray(e.displayText().c_str())));
			return false;
		}
		return true;
	}
}

QStringList UploadThread::FTP_GetDirectories()
{
	QStringList listsource;
	try
	{
		std::string str;
		std::istream& ftpin = this->ftpsession->beginList("", true);

		while (ftpin >> str)
		{
			listsource.append(QTextCodec::codecForLocale()->toUnicode(QByteArray(str.c_str())));
		}

		this->ftpsession->endList();
	}
	catch (Poco::Net::FTPException& e)
	{
		emit this->exceptionMessage(QTextCodec::codecForLocale()->toUnicode(QByteArray(e.displayText().c_str())));
	}
	return this->parseDirs(listsource);
}

QStringList UploadThread::FTP_GetFiles()
{
	QStringList listsource;
	try
	{
		std::string str;
		std::istream& ftpin = this->ftpsession->beginList("", true);

		while (ftpin >> str)
		{
			listsource.append(QTextCodec::codecForLocale()->toUnicode(QByteArray(str.c_str())));
		}

		this->ftpsession->endList();
	}
	catch (Poco::Net::FTPException& e)
	{
		emit this->exceptionMessage(QTextCodec::codecForLocale()->toUnicode(QByteArray(e.displayText().c_str())));
	}
	return this->parseFiles(listsource);
}

QStringList UploadThread::parseFiles(QStringList list) const
{
	QStringList filelist;
	for (int i = 0; (i * 9) < list.size(); i++) {
		if (!list.at(i * 9).startsWith("d", Qt::CaseSensitive)) {
			if (((i * 9) + 8) < list.size()) {
				filelist.append(list.at((i * 9) + 8));
			}
		}
	}
	return filelist;
}

QStringList UploadThread::parseDirs(QStringList list) const
{
	QStringList dirlist;
	for (int i = 0; (i * 9) < list.size(); i++) {
		if (list.at(i * 9).startsWith("d", Qt::CaseSensitive)) {
			if (((i * 9) + 8) < list.size()) {
				if ((list.at((i * 9) + 8) != ".") && (list.at((i * 9) + 8) != "..")) {
					dirlist.append(list.at((i * 9) + 8));
				}
			}
		}
	}
	return dirlist;
}

void UploadThread::setUploadFileList(QStringList filelist)
{
	this->filelist = filelist;
}

size_t UploadThread::getFileSize(QString filename)
{
	QStringList listsource;
	try
	{
		std::string str;
		std::istream& ftpin = this->ftpsession->beginList("", true);

		while (ftpin >> str)
		{
			listsource.append(QTextCodec::codecForLocale()->toUnicode(QByteArray(str.c_str())));
		}

		this->ftpsession->endList();
	}
	catch (Poco::Net::FTPException& e)
	{
		emit this->exceptionMessage(QTextCodec::codecForLocale()->toUnicode(QByteArray(e.displayText().c_str())));
	}
	for (int i = 0; (i * 9) < listsource.size(); i++) {
		if (!listsource.at(i).startsWith('d', Qt::CaseSensitive)) {
			if (((i * 9) + 8) < listsource.size()) {
				if (listsource.at((i * 9) + 8) == filename) {
					return listsource.at((i * 9) + 4).toUInt();
				}
			}
		}
	}
	return 0;
}

void UploadThread::run()
{
	for (int i = 0; i < this->filelist.size(); i++) {
		QString filename = this->filelist.at(i);
		QFile file(filename);
		if (file.open(QIODevice::ReadOnly)) {
			QByteArray filedata = file.readAll();
			file.close();

			QString absoluteFileName = filename.right(filename.length() - filename.lastIndexOf('/') - 1);
			absoluteFileName.replace(' ', '-', Qt::CaseSensitive);

			bool fileok = true;
			try
			{
				std::ostream& ftpout = this->ftpsession->beginUpload(std::string(QTextCodec::codecForLocale()->fromUnicode(absoluteFileName).constData()));
				ftpout.write(filedata.constData(), filedata.size());
				this->ftpsession->endUpload();
			}
			catch (Poco::Net::FTPException& e)
			{
				fileok = false;
				emit this->exceptionMessage(QTextCodec::codecForLocale()->toUnicode(QByteArray(e.displayText().c_str())));
				emit this->exceptionMessage("上传文件出错:" + filename);
			}

			if (fileok) {
				int filesizetrue = this->getFileSize(absoluteFileName);
				int filesize = filedata.size();

				if (this->getFileSize(absoluteFileName) == filedata.size()) {
					emit this->exceptionMessage("已上传文件:" + filename);
				}
				else {
					emit this->exceptionMessage("服务器端文件与本地文件大小不同，请手动校验:" + filename);
				}
			}
		}
		else {
			emit this->exceptionMessage("无法读取文件:" + filename);
		}
	}

	emit this->finished();
}

void UploadThread::FTP_Close()
{
	try
	{
		this->ftpsession->close();
	}
	catch (Poco::Net::FTPException& e)
	{
		emit this->exceptionMessage(QTextCodec::codecForLocale()->toUnicode(QByteArray(e.displayText().c_str())));
	}
	emit this->exceptionMessage("FTP通信结束");
}
