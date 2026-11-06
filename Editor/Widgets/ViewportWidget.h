#pragma once
#include <qwidget.h>
#include <Windows.h>
#include "Engine.h"

class ViewportWidget : public QWidget
{
public:
	ViewportWidget(QWidget* parent = nullptr);
	HWND GetHWND() const {
		return reinterpret_cast<HWND>(this->winId());
	}
	void SetEngine(Engine* engine) {
		this->engine = engine;
	};
protected:
	void keyPressEvent(QKeyEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;

private:
	Engine* engine;
	QPointF prevPos;
	bool hasPrevPos = false;
};

