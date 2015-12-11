#include "mainwin.h"
#include <random>
#include <time.h>
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	srand(time(NULL));
	QApplication a(argc, argv);
	a.setQuitOnLastWindowClosed(true);
	MainWin w;
	w.show();

	//a.quitOnLastWindowClosed();
	a.exec();

	return 0;
}
