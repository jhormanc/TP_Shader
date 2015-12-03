#include "mainwin.h"

MainWin::MainWin(QWidget *parent)
	: QMainWindow(parent)
{

	ui.setupUi(this);
	renderer.render();
}

MainWin::~MainWin()
{

}
