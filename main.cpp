#include "AutomaticXuanShanLi.h"
#include <QtWidgets/QApplication>
#include <QDir>

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	QDir::setCurrent(QDir::homePath());

	AutomaticXuanShanLi w;
	w.show();
	return a.exec();
}
