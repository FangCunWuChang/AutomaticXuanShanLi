#pragma once

#include <QThread>
#include "Poco/Net/FTPClientSession.h"
#include "Poco/Exception.h"
#include "Poco/Net/NetException.h"
#include <QStringList>
#include <string>
#include <vector>
#include <iostream>
#include <QFile>
#include <QByteArray>
#include <QTextCodec>

class UploadThread : public QThread
{
	Q_OBJECT

public:
	UploadThread();
	~UploadThread();

	bool FTP_Open(QString address, uint port);
	bool FTP_Login(QString user, QString password);

	bool FTP_SetDirectory(QString dir);
	bool FTP_AddDirectory(QString dir);

	QStringList FTP_GetDirectories();
	QStringList FTP_GetFiles();

	void setUploadFileList(QStringList filelist);

	void FTP_Close();
private:
	Poco::Net::FTPClientSession* ftpsession = nullptr;

	QStringList parseFiles(QStringList list) const;
	QStringList parseDirs(QStringList list) const;

	QStringList filelist;

	size_t getFileSize(QString filename);
protected:
	void run();
signals:
	void exceptionMessage(QString message);
	void finished();
};
