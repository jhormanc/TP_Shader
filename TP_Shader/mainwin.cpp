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
	refresh = false;

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

	QString text, text2, text3, text4, text5, text6, text7;
	int textWidth, textWidth2, textWidth3, textWidth4, textWidth5, textWidth6, textWidth7;
	QFontMetrics metrics = painter.fontMetrics();

	if (!rendering || refresh)
	{
		text = QString("ZQSD and AE keys to move camera. "
			"Left mouse click to rotate camera.");
		text2 = QString("Space to change rendering mode. "
			"N to precalculate. "
			"+ / - to change samples.");
		text3 = QString("Mode = %1 - Samples = %2 - Sun = [%3 %4 %5] - Delta R = %6").arg(
			QString(Renderer::renderPrecalculed ? "Precalculated" : "Real-time"),
			QString::number(Renderer::nbSamples),
			QString::number(Renderer::sunPoint.x),
			QString::number(Renderer::sunPoint.y),
			QString::number(Renderer::sunPoint.z),
			QString::number(Renderer::rDelta));
		text4 = QString("Intensity : Sun = %1, Global = %2").arg(
			QString::number(Renderer::sunIntensity),
			QString::number(Renderer::globalIntensity));
		text5 = QString("Coeff : Diffus = %1, Specular = %2").arg(
			QString::number(Renderer::coefDiffus),
			QString::number(Renderer::coefSpec));
		text6 = QString("Influence : Sun = %1, Specular = %2").arg(
			QString::number(Renderer::sunInfluence),
			QString::number(Renderer::specInfluence));

		float t = thread.GetRenderTime();
		text7 = QString("FPS : %1").arg(QString::number(t != 0.f ? 1.f / t : 0.f));

		textWidth2 = metrics.width(text2);
		textWidth3 = metrics.width(text3);
		textWidth4 = metrics.width(text4);
		textWidth5 = metrics.width(text5);
		textWidth6 = metrics.width(text6);
		textWidth7 = metrics.width(text7);
	}
	else
		text = tr("RENDERING, PLEASE WAIT...");

	
	textWidth = metrics.width(text);
	painter.setPen(Qt::white);
	painter.drawText((width() - textWidth) / 2, metrics.leading() + metrics.ascent(), text);

	if (!rendering || refresh)
	{
		painter.drawText((width() - textWidth2) / 2, metrics.leading() + metrics.ascent() + 15, text2);
		painter.drawText((width() - textWidth3) / 2, metrics.leading() + metrics.ascent() + 30, text3);
		painter.drawText((width() - textWidth4) / 2, metrics.leading() + metrics.ascent() + 45, text4);
		painter.drawText((width() - textWidth5) / 2, metrics.leading() + metrics.ascent() + 60, text5);
		painter.drawText((width() - textWidth6) / 2, metrics.leading() + metrics.ascent() + 75, text6);
		painter.drawText((width() - textWidth7) / 2, metrics.leading() + metrics.ascent() + 90, text7);
	}
}

void MainWin::resizeEvent(QResizeEvent * /* event */)
{
	thread.render();
}

void MainWin::keyPressEvent(QKeyEvent *event)
{
	if (thread.refreshAuto && event->key() == Qt::Key_C)
		changeRenderAuto();
	else if (!rendering)
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
			changeNbSamples(samplesStep);
			break;
		case Qt::Key_Minus:
			changeNbSamples(-samplesStep);
			break;
		case Qt::Key_Space:
			changeRenderMode();
			break;
		case Qt::Key_P:
			addCoeff(true, coeffStep);
			break;
		case Qt::Key_M:
			addCoeff(true, -coeffStep);
			break;
		case Qt::Key_O:
			addCoeff(false, coeffStep);
			break;
		case Qt::Key_L:
			addCoeff(false, -coeffStep);
			break;
		case Qt::Key_I:
			addIntensity(intensityStep);
			break;
		case Qt::Key_K:
			addIntensity(-intensityStep);
			break;
		case Qt::Key_U:
			addInfluence(true, influenceStep);
			break;
		case Qt::Key_J:
			addInfluence(true, -influenceStep);
			break;
		case Qt::Key_Y:
			addInfluence(false, influenceStep);
			break;
		case Qt::Key_H:
			addInfluence(false, -influenceStep);
			break;
		case Qt::Key_T:
			addDeltaR(stepDeltaR);
			break;
		case Qt::Key_G:
			addDeltaR(-stepDeltaR);
			break;
		case Qt::Key_Up:
			moveSun(0.f, sunMoveStep, 0.f);
			break;
		case Qt::Key_Down:
			moveSun(0.f, -sunMoveStep, 0.f);
			break;
		case Qt::Key_Left:
			moveSun(-sunMoveStep, 0.f, 0.f);
			break;
		case Qt::Key_Right:
			moveSun(sunMoveStep, 0.f, 0.f);
			break;
		case Qt::Key_N:
			updatePrecalc();
			break;
		case Qt::Key_W:
			changeRenderColor();
			break;
		case Qt::Key_X:
			changeRenderIter();
			break;
		case Qt::Key_C:
			changeRenderAuto();
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
	if (thread.changeNbSamples(nbToAdd) && !Renderer::renderPrecalculed)
	{
		rendering = true;
		refresh = true;
		update();
		thread.render();
	}

	update();
}

void MainWin::changeRenderMode()
{
	thread.changeRenderMode(); 
	rendering = true;
	refresh = false;
	update();
	if (Renderer::renderPrecalculed && !thread.calledPrecalc)
	{
		repaint();
		thread.UpdatePrecalc();
		thread.calledPrecalc = true;
	}

	thread.render();
}

void MainWin::updatePrecalc()
{
	if (Renderer::renderPrecalculed)
	{
		rendering = true;
		refresh = false;
		update();
		repaint();
		thread.UpdatePrecalc();
		thread.render();
	}
}

void MainWin::addCoeff(const bool& diffus, const float& coefToAdd)
{
	if (thread.AddCoeff(diffus, coefToAdd))
	{
		rendering = true;
		refresh = true;
		update();
		thread.render();
	}
}

void MainWin::addIntensity(const float& intensityToAdd)
{
	if (thread.AddIntensity(intensityToAdd))
	{
		rendering = true;
		refresh = true;
		update();
		thread.render();
	}
}

void MainWin::addInfluence(const bool& sun, const int& influenceToAdd)
{
	if (thread.AddInfluence(sun, sun ? influenceToAdd : influenceToAdd * 5))
	{
		rendering = true;
		refresh = true;
		update();
		thread.render();
	}
}

void MainWin::moveSun(const float& x, const float& y, const float& z)
{
	rendering = true;
	refresh = true;
	thread.MoveSun(Vector(x, y, z));
	update();
	thread.render();
}

void MainWin::addDeltaR(const float& delta)
{
	if (thread.AddDeltaR(delta))
	{
		rendering = true;
		refresh = true;
		update();
		thread.render();
	}
}

void MainWin::changeRenderColor()
{
	rendering = true;
	refresh = true;
	thread.ChangeRenderColor();
	update();
	thread.render();
}

void MainWin::changeRenderIter()
{
	rendering = true;
	refresh = true;
	thread.ChangeRenderIter();
	update();
	thread.render();
}

void MainWin::changeRenderAuto()
{
	rendering = true;
	refresh = true;
	thread.ChangeRenderAuto();
	update();
	thread.render();
}

void MainWin::closeEvent(QCloseEvent *event)
{
	thread.abort = true;
	thread.exit();
	event->accept();
}

