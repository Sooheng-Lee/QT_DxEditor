#include "ViewportWidget.h"

ViewportWidget::ViewportWidget(QWidget* parent) : QWidget(nullptr)
{
	setAttribute(Qt::WA_NativeWindow);
	setAttribute(Qt::WA_PaintOnScreen);
	setAttribute(Qt::WA_NoSystemBackground);
	setMinimumSize(320, 240);
}
