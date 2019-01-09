#include "utils.h"

#include <math.h>


//https://www.particleincell.com/wp-content/uploads/2013/08/cubic-line.svg


wgs_int8 sgn(wgs_float val) {
	return (0.f < val) - (val < 0.f);
}

wgs_vec4f bezierCoeffs(wgs_float c0, wgs_float c1, wgs_float c2, wgs_float c3)
{
	return{
		-c0 + 3.f * c1 + -3.f * c2 + c3,
		3.f * c0 - 6.f * c1 + 3.f * c2,
		-3.f * c0 + 3.f * c1,
		c0
	};
}

wgs_vec4f getCubicRoots(wgs_vec4f coef)
{
	wgs_float a = coef.y / coef.x;
	wgs_float b = coef.z / coef.x;
	wgs_float c = coef.w / coef.x;

	wgs_float q = (3.f * b - powf(a, 2.f)) / 9.f;
	wgs_float r = (9.f * a*b - 27.f * c - 2.f * powf(a, 3.f)) / 54.f;
	wgs_float d = powf(q, 3.f) + powf(r, 2.f);

	wgs_float sqrt_d = sqrtf(d);

	if (d >= 0.f)
	{
		wgs_float s = sgn(r + sqrt_d)*powf(fabsf(r + sqrt_d), (1.f / 3.f));
		wgs_float t = sgn(r - sqrt_d)*powf(fabsf(r - sqrt_d), (1.f / 3.f));

		if (fabsf(sqrtf(3.f)*(s - t) / 2.f) > 0.000001f)
			return{ -a / 3 + (s + t), -1.f, -1.f };

		return{ 
			-a / 3 + (s + t),
			-a / 3 - (s + t) / 2,
			-a / 3 - (s + t) / 2,
			-1.f
		};
	}
	else
	{
		wgs_float th = acosf(r / sqrtf(-powf(q, 3.f)));
		wgs_float sqrt_mq = sqrtf(-q);
		return{
			2 * sqrt_mq*cosf(th / 3) - a / 3,
			2 * sqrt_mq*cosf((th + 2 * 3.14159265f) / 3) - a / 3,
			2 * sqrt_mq*cosf((th + 4 * 3.14159265f) / 3) - a / 3,
			-1.f
		};
	}
	return{ -1.f, -1.f, -1.f, -1.f };
}

bool intersect_cubic_spline_line(wgs_vec2f s0, wgs_vec2f s1, wgs_vec2f s2, wgs_vec2f s3, wgs_vec2f l0, wgs_vec2f l1)
{
	wgs_vec4f bx = bezierCoeffs(s0.x, s1.x, s2.x, s3.x);
	wgs_vec4f by = bezierCoeffs(s0.y, s1.y, s2.y, s3.y);

	wgs_float A = l1.y - l0.y;
	wgs_float B = l0.x - l1.x;
	wgs_float C = l0.x * (l0.y - l1.y) +
		l0.y * (l1.x - l0.x);

	wgs_vec4f coefs = {
		A*bx.x + B*by.x,
		A*bx.y + B*by.y,
		A*bx.z + B*by.z,
		A*bx.w + B*by.w + C
	};
	wgs_vec4f roots = getCubicRoots(coefs);

	for (wgs_uint8 i = 0; i < 3; i++)
	{
		wgs_float t = ((wgs_float*)&roots)[i];

		if (0.f < t && t <= 1.f)
		{
			wgs_float c0 = bx.x * t*t*t + bx.y * t*t + bx.z * t + bx.w;
			wgs_float c1 = by.x * t*t*t + by.y * t*t + by.z * t + by.w;

			wgs_float s;
			if (fabsf(l1.x - l0.x) > 0.000001f)
				s = (c0 - l0.x) / (l1.x - l0.x);
			else
				s = (c1 - l0.y) / (l1.y - l0.y);

			if (0.f < s && s <= 1.f)
				return wgs_true;
		}
	}
	return wgs_false;
}
