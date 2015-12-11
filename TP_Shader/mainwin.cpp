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
	connect(&thread, SIGNAL(renderedImage(QImage)), this, SLOT(updatePixmap(QImage)));

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

	painter.drawPixmap(QPoint(), pixmap);

	QString text = tr("Use ZQSD or the '+' and '-' keys move camera"
		"Left mouse click to rotate camera");
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
	switch (event->key()) 
	{
	case Qt::Key_Z:
		move(0, 100, 0);
		break;
	case Qt::Key_S:
		move(0, -100, 0);
		break;
	case Qt::Key_Q:
		move(-100, 0, 0);
		break;
	case Qt::Key_D:
		move(100, 0, 0);
		break;
	case Qt::Key_Plus:
		move(0, 0, 100);
		break;
	case Qt::Key_Minus:
		move(0, 0, -100);
		break;
	default:
		QWidget::keyPressEvent(event);
	}
}

#ifndef QT_NO_WHEELEVENT
void MainWin::wheelEvent(QWheelEvent *event)
{

}
#endif

void MainWin::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		rotate(event->pos());
	}
}

void MainWin::mouseMoveEvent(QMouseEvent *event)
{
	// Pour du vrai temps réel !
	//rotate(event->pos());
}

void MainWin::mouseReleaseEvent(QMouseEvent *event)
{

}

void MainWin::updatePixmap(const QImage &image)
{
	pixmap = QPixmap::fromImage(image);
	update();
}

void MainWin::move(const int& x, const int& y, const int& z)
{
	thread.MoveCam(x, y, z);
	update();
	thread.render();
}

void MainWin::rotate(const QPoint& pt)
{
	thread.RotateCam(Point(pt.x(), pt.y(), 0.f));
	update();
	thread.render();
}

void MainWin::closeEvent(QCloseEvent *event)
{

	thread.abort = true;
	thread.exit();
	event->accept();
}