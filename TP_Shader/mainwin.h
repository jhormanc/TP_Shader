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
	bool refresh;

	void rotate(const QPoint& pt);
	void move(const int& x = 0, const int& y = 0, const int& z = 0);
	void changeNbSamples(const int& nbToAdd);
	void changeRenderMode();
	void updatePrecalc();
	// diffus : diffus ou spéculaire
	// coefToAdd : entre 0.f et 1.f
	void addCoeff(const bool& diffus, const float& coefToAdd);
	// intensityToAdd : 0.f < globalIntensity + sunIntensity < 1.f
	void addIntensity(const float& intensityToAdd);
	// sun : soleil ou spéculaire
	// influenceToAdd : 10 < influenceSpec < 40
	void addInfluence(const bool& sun, const int& influenceToAdd);
	void closeEvent(QCloseEvent *event);
	void moveSun(const float& x, const float& y, const float& z);
};

#endif // MAINWIN_H
