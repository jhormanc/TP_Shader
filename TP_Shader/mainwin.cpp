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

	rendering = false;

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

	QString text, text2, text3;
	int textWidth, textWidth2, textWidth3;
	QFontMetrics metrics = painter.fontMetrics();

	if (rendering == false)
	{
		text = QString("Use ZQSD and AE keys to move camera. "
			"Left mouse click to rotate camera.");
		text2 = QString("M to change rendering mode. U to precalculate. "
			"+ / - to changes samples");
		text3 = QString("Mode : %1 - Samples : %2").arg(
			QString(thread.IsRenderPrecalc() ? "Precalculated" : "Real-time"),
			QString::number(thread.GetNbSamples()));

		textWidth2 = metrics.width(text2);
		textWidth3 = metrics.width(text3);
	}
	else
		text = tr("RENDERING, PLEASE WAIT...");

	
	textWidth = metrics.width(text);


	painter.setPen(Qt::NoPen);
	painter.setBrush(QColor(0, 0, 0, 127));
	painter.drawRect((width() - textWidth) / 2 - 5, 0, textWidth + 10, metrics.lineSpacing() + 5);
	painter.setPen(Qt::white);
	painter.drawText((width() - textWidth) / 2, metrics.leading() + metrics.ascent(), text);

	if (rendering == false)
	{
		painter.drawText((width() - textWidth2) / 2, metrics.leading() + metrics.ascent() + 15, text2);
		painter.drawText((width() - textWidth3) / 2, metrics.leading() + metrics.ascent() + 30, text3);
	}
}

void MainWin::resizeEvent(QResizeEvent * /* event */)
{
	thread.render();
}

void MainWin::keyPressEvent(QKeyEvent *event)
{
	if (!rendering)
	{
		switch (event->key())
		{
		case Qt::Key_Z:
			move(0, moveStep, 0);
			break;
		case Qt::Key_S:
			move(0, -moveStep, 0);
			break;
		case Qt::Key_Q:
			move(-moveStep, 0, 0);
			break;
		case Qt::Key_D:
			move(moveStep, 0, 0);
			break;
		case Qt::Key_E:
			move(0, 0, moveStep);
			break;
		case Qt::Key_A:
			move(0, 0, -moveStep);
			break;
		case Qt::Key_Plus:
			changeNbSamples(1);
			break;
		case Qt::Key_Minus:
			changeNbSamples(-1);
			break;
		case Qt::Key_M:
			changeRenderMode();
			break;
		case Qt::Key_U:
			updatePrecalc();
			break;
		default:
			QWidget::keyPressEvent(event);
		}
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
	//rotate(event->pos());
}

void MainWin::updatePixmap(const QImage &image)
{
	pixmap = QPixmap::fromImage(image);
	rendering = false;
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

void MainWin::changeNbSamples(const int& nbToAdd)
{
	if (thread.changeNbSamples(nbToAdd) && !thread.IsRenderPrecalc())
	{
		rendering = true;
		update();
		thread.render();
	}

	update();
}

void MainWin::changeRenderMode()
{
	thread.changeRenderMode(); 
	rendering = true;
	update();
	if (thread.IsRenderPrecalc() && !thread.calledPrecalc)
	{
		repaint();
		thread.UpdatePrecalc();
		thread.calledPrecalc = true;
	}

	thread.render();
}

void MainWin::updatePrecalc()
{
	if (thread.IsRenderPrecalc())
	{
		rendering = true;		
		update();
		repaint();
		thread.UpdatePrecalc();
		thread.render();
	}
}

void MainWin::closeEvent(QCloseEvent *event)
{
	thread.abort = true;
	thread.exit();
	event->accept();
}