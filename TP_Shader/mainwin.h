#ifndef MAINWIN_H
#define MAINWIN_H

#include <QPixmap>
#include <QWidget>
#include <QObject>
#include "Renderer.h"
#include "Global\Constants.h"

class MainWin : public QWidget
{
	Q_OBJECT

public:
	MainWin(QWidget *parent = 0);
protected:
	void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
	void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
	void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
#ifndef QT_NO_WHEELEVENT
	void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
#endif
	void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
	void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
	void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

	private slots:
	void updatePixmap(const QImage &image);

private:
	Renderer thread;
	QPixmap pixmap;
	bool rendering;

	void rotate(const QPoint& pt);
	void move(const int& x = 0, const int& y = 0, const int& z = 0);
	void changeNbSamples(const int& nbToAdd);
	void changeRenderMode();
	void updatePrecalc();
	void closeEvent(QCloseEvent *event);
};

#endif // MAINWIN_H
