#include <string.h>

void lpf(float new_, float *filtered, float a)
{
	*filtered = (new_ * a) + (*filtered * (1.0f - a));
}
