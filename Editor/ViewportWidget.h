#pragma once
#include <qwidget.h>
#include <Windows.h>
class ViewportWidget : public QWidget
{
public:
	ViewportWidget(QWidget* parent = nullptr);
	HWND GetHWND() const {
		return reinterpret_cast<HWND>(this->winId());
	}
};

