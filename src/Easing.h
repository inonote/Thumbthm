#pragma once
#include "stdafx.h"

namespace Easing {
	double liner(double t);
	double easeInQuad(double t);
	double easeOutQuad(double t);
	double easeInOutQuad(double t);
	double easeInCubic(double t);
	double easeOutCubic(double t);
	double easeInOutCubic(double t);
	double easeInQuart(double t);
	double easeOutQuart(double t);
	double easeInOutQuart(double t);
	double easeInQuint(double t);
	double easeOutQuint(double t);
	double easeInOutQuint(double t);

	double easeOutBack(double t);
	double easeInOutBack(double t);

}