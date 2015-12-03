#ifndef MAINWIN_H
#define MAINWIN_H

#include <QPixmap>
#include <QWidget>
#include <QObject>
#include "Renderer.h"

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
	void updatePixmap(const QImage &image, float scaleFactor);
	void zoom(float zoomFactor);

private:
	void scroll(int deltaX, int deltaY);

	Renderer thread;
	QPixmap pixmap;
	QPoint pixmapOffset;
	QPoint lastDragPos;
};

#endif // MAINWIN_H
