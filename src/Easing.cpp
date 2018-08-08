#include "stdafx.h"
#include "Easing.h"
#include <math.h>
#define PI 3.1415926545

namespace Easing {
	double liner(double t) {
		return t;
	}
	double easeInQuad(double t) {
		return t * t;
	}
	double easeOutQuad(double t) {
		return t * (2 - t);
	}
	double easeInOutQuad(double t) {
		return t < .5 ? 2 * t * t : -1 + (4 - 2 * t) * t;
	}
	double easeInCubic(double t) {
		return t * t * t;
	}
	double easeOutCubic(double t) {
		return (--t)* t * t + 1;
	}
	double easeInOutCubic(double t) {
		return t < .5 ? 4 * t * t * t : (t - 1) * (2 * t - 2) * (2 * t - 2) + 1;
	}
	double easeInQuart(double t) {
		return t * t * t * t;
	}
	double easeOutQuart(double t) {
		return 1 - (--t) * t * t * t;
	}
	double easeInOutQuart(double t) {
		return t < .5 ? 8 * t * t * t * t : 1 - 8 * (--t) * t * t * t;
	}
	double easeInQuint(double t) {
		return t * t * t * t * t;
	}
	double easeOutQuint(double t) {
		return 1 + (--t) * t * t * t * t;
	}
	double easeInOutQuint(double t) {
		return t < .5 ? 16 * t * t * t * t * t : 1 + 16 * (--t) * t * t * t * t;
	}

	double easeOutBack(double t) {
		return 1 + (--t) * t * (2.70158 * t + 1.70158);
	}
	double easeInOutBack(double t) {
		if (t < 0.5) {
			return t * t * (7 * t - 2.5) * 2;
		}
		else {
			return 1 + (--t) * t * 2 * (7 * t + 2.5);
		}
	}

}