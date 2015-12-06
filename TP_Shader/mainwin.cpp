#include <QPainter>
#include <QKeyEvent>

#include <math.h>
#include "mainwin.h"

const float ZoomInFactor =  0.8f;
const float ZoomOutFactor = 1.f / ZoomInFactor;
const int ScrollStep = 20;

MainWin::MainWin(QWidget *parent)
	: QWidget(parent)
{
	connect(&thread, SIGNAL(renderedImage(QImage, float)), this, SLOT(updatePixmap(QImage, float)));

	setWindowTitle(tr("Shader"));
#ifndef QT_NO_CURSOR
	setCursor(Qt::CrossCursor);
#endif
	resize(768, 768);
}

void MainWin::paintEvent(QPaintEvent * /* event */)
{
	QPainter painter(this);
	painter.fillRect(rect(), Qt::black);

	if (pixmap.isNull()) 
	{
		painter.setPen(Qt::white);
		painter.drawText(rect(), Qt::AlignCenter, tr("Rendering initial image, please wait..."));
		return;
	}

	painter.drawPixmap(pixmapOffset, pixmap);

	QString text = tr("Use mouse wheel or the '+' and '-' keys to zoom. "
		"Press and hold left mouse button to scroll.");
	QFontMetrics metrics = painter.fontMetrics();
	int textWidth = metrics.width(text);

	painter.setPen(Qt::NoPen);
	painter.setBrush(QColor(0, 0, 0, 127));
	painter.drawRect((width() - textWidth) / 2 - 5, 0, textWidth + 10, metrics.lineSpacing() + 5);
	painter.setPen(Qt::white);
	painter.drawText((width() - textWidth) / 2, metrics.leading() + metrics.ascent(), text);
}

void MainWin::resizeEvent(QResizeEvent * /* event */)
{
	thread.render();
}

void MainWin::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) {
	case Qt::Key_Plus:
		zoom(ZoomInFactor);
		break;
	case Qt::Key_Minus:
		zoom(ZoomOutFactor);
		break;
	case Qt::Key_Left:
		thread.CameraRotation(1);
		scroll(ScrollStep, 0);
		break;
	case Qt::Key_Right:
		thread.CameraRotation(-1);
		scroll(ScrollStep, 0);
		break;
	case Qt::Key_Down:
		thread.CameraUp(-100);
		scroll(0, ScrollStep);
		break;
	case Qt::Key_Up:
		thread.CameraUp(100);
		scroll(0, ScrollStep);
		break;
	default:
		QWidget::keyPressEvent(event);
	}
}

#ifndef QT_NO_WHEELEVENT
void MainWin::wheelEvent(QWheelEvent *event)
{
	int numDegrees = event->delta() / 8;
	double numSteps = numDegrees / 15.0f;
	zoom(pow(ZoomInFactor, numSteps));
}
#endif

void MainWin::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
		lastDragPos = event->pos();
}

void MainWin::mouseMoveEvent(QMouseEvent *event)
{
	if (event->buttons() & Qt::LeftButton) {
		pixmapOffset += event->pos() - lastDragPos;
		lastDragPos = event->pos();
		update();
	}
}

void MainWin::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton) 
	{
		pixmapOffset += event->pos() - lastDragPos;
		lastDragPos = QPoint();

		int deltaX = (width() - pixmap.width()) / 2 - pixmapOffset.x();
		int deltaY = (height() - pixmap.height()) / 2 - pixmapOffset.y();
		scroll(deltaX, deltaY);
	}
}

void MainWin::updatePixmap(const QImage &image, float scaleFactor)
{
	if (!lastDragPos.isNull())
		return;

	pixmap = QPixmap::fromImage(image);
	pixmapOffset = QPoint();
	lastDragPos = QPoint();
	update();
}

void MainWin::zoom(float zoomFactor)
{
	// TODO
	update();
	thread.render();
}

void MainWin::scroll(int deltaX, int deltaY)
{
	// TODO
	update();
	thread.render();
}