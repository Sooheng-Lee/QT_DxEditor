#pragma once
#include "Objects/BaseObject.h"
#include <qstring.h>
#include <vector>
#include <memory>
#include <cstdint>

struct ObjectNode
{
	uint64_t ID = 0;
	QString name = "GameObject";
	BaseObject object;
};