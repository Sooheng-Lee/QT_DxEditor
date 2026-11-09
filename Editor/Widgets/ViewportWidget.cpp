#include "ViewportWidget.h"
#include <QKeyEvent>

ViewportWidget::ViewportWidget(QWidget* parent) : QWidget(parent)
{
	setAttribute(Qt::WA_NativeWindow);
	setAttribute(Qt::WA_PaintOnScreen);
	setAttribute(Qt::WA_NoSystemBackground);
	setMinimumSize(320, 240);
	setFocusPolicy(Qt::StrongFocus);
}

void ViewportWidget::keyPressEvent(QKeyEvent* event)
{
	if (!engine) { QWidget::keyPressEvent(event); return; }
	switch (event->key())
	{
	case Qt::Key_W:
		qDebug() << "W Pressed";
		engine->CameraMove(0.0f, 0.0f, 0.1f);
		break;

	case Qt::Key_S:
		qDebug() << "S Pressed";
		engine->CameraMove(0.0f, 0.0f, -0.1f);
		break;
	case Qt::Key_D:
		qDebug() << "D Pressed";
		engine->CameraMove(0.1f, 0.0f, 0.0f);
		break;
	case Qt::Key_A:
		qDebug() << "A Pressed";
		engine->CameraMove(-0.1f, 0.0f, 0.0f);
		break;
	}

	QWidget::keyPressEvent(event);
}

void ViewportWidget::mouseMoveEvent(QMouseEvent* event)
{
	if (!engine) return;
	QPointF currentPos = event->position();
	if (hasPrevPos)
	{
		QPointF delta = currentPos - prevPos;
		engine->CameraRotate(-delta.y(), delta.x(), 0);
	}

	prevPos = currentPos;
	hasPrevPos = true;
}

void ViewportWidget::mouseReleaseEvent(QMouseEvent* event)
{
	hasPrevPos = false;
	QWidget::mouseReleaseEvent(event);
}
