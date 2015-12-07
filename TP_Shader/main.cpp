#include "mainwin.h"
#include <random>
#include <time.h>
#include <QtWidgets/QApplication>


int main(int argc, char *argv[])
{
	srand(time(NULL));
	QApplication a(argc, argv);
	MainWin w;
	w.show();

	a.exec();

	return 0;
}
