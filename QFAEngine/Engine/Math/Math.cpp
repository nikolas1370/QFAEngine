#include "Math.h"

const glm::mat4 Math::DefauldMatrix4 =
{1, 0, 0, 0,
 0, 1, 0, 0,
 0, 0, 1, 0,
 0, 0, 0, 1};

const glm::mat3 Math::DefauldMatrix3 =
{
	1, 0, 0,
	0, 1, 0,
	0, 0, 1
};

glm::mat3 Math::rotateMatrix3(glm::mat3 const& m, float angle, glm::vec3 const& v)
{
	float const a = angle;
	float const c = cos(a);
	float const s = sin(a);

	glm::vec3 axis(normalize(v));
	glm::vec3 temp((float(1) - c) * axis);

	glm::mat3 Rotate;
	Rotate[0][0] = c + temp[0] * axis[0];
	Rotate[0][1] = temp[0] * axis[1] + s * axis[2];
	Rotate[0][2] = temp[0] * axis[2] - s * axis[1];

	Rotate[1][0] = temp[1] * axis[0] - s * axis[2];
	Rotate[1][1] = c + temp[1] * axis[1];
	Rotate[1][2] = temp[1] * axis[2] + s * axis[0];

	Rotate[2][0] = temp[2] * axis[0] + s * axis[1];
	Rotate[2][1] = temp[2] * axis[1] - s * axis[0];
	Rotate[2][2] = c + temp[2] * axis[2];

	glm::mat3  Result;
	Result[0] = m[0] * Rotate[0][0] + m[1] * Rotate[0][1] + m[2] * Rotate[0][2];
	Result[1] = m[0] * Rotate[1][0] + m[1] * Rotate[1][1] + m[2] * Rotate[1][2];
	Result[2] = m[0] * Rotate[2][0] + m[1] * Rotate[2][1] + m[2] * Rotate[2][2];
	return Result;

}

glm::mat3  Math::ScaleMatrix3(glm::mat3 const& m, glm::vec3 const& v)
{
	glm::mat3 Result;
	Result[0] = m[0] * v[0];
	Result[1] = m[1] * v[1];
	Result[2] = m[2] * v[2];
	return Result;
}

/* if need Linear To SRGB
vec3 LinearToSRGB(vec3 rgb)
{
  // See https://gamedev.stackexchange.com/questions/92015/optimized-linear-to-srgb-glsl
  return mix(1.055 * pow(rgb, vec3(1.0 / 2.4)) - 0.055,
			 rgb * 12.92,
			 lessThanEqual(rgb, vec3(0.0031308)));
}
*/

// from https://gist.github.com/Reedbeta/e8d3817e3f64bba7104b8fafd62906df
glm::vec3 Math::srgb_to_linear(glm::vec3 rgb)
{
	return glm::mix(glm::pow((rgb + 0.055f) * (1.0f / 1.055f), glm::vec3(2.4f)),
		rgb * (1.0f / 12.92f),
		glm::lessThanEqual(rgb, glm::vec3(0.04045f)));
}