#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QByteArray>
#include <QStringList>
#include "IFtpType.h"

class IFtp : public QObject
{
	Q_OBJECT

public:
	IFtp(QObject* parent);
	~IFtp();

	void Ftp_init(QString address, uint port, QString user, QString password);
	void Ftp_Connect();
	void Ftp_Send(QString command);

	void Ftp_cd(QString path);

private:
	QTcpSocket* commandSocket = nullptr;
	QTcpSocket* dataSocket = nullptr;

	QString address;
	uint port = 21;
	QString user = "anonymous";
	QString password;

	void parseResult(QString result);

	void ErrorFunc();

private slots:
	void on_csConnect();
	void on_csReadyRead();

signals:
	void connected();
	void error();
	void message(QString message);
};
