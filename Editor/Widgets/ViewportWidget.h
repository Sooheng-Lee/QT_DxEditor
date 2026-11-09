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
	void ResetMousePosition() { hasPrevPos = false; }
protected:
	void keyPressEvent(QKeyEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;

private:
	Engine* engine = nullptr;
	QPointF prevPos;
	bool hasPrevPos = false;
};

