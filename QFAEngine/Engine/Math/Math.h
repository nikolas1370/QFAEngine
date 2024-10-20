#pragma once
#define PI 					(3.1415926535897932f)	
#define DOUBLE_PI		    (3.141592653589793238462643383279502884197169399)

#define EqualFloats(value1, value2) (abs((value1) - (value2)) < 0.00001)
#define EqualToZero(value) (abs(value) < 0.00001)
#define NotEqualToZero(value) !(abs(value) < 0.00001)

#include <Tools/Stuff.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class QFAEXPORT Math
{
public:
	static const glm::mat4 DefauldMatrix4;
	static const glm::mat3 DefauldMatrix3;
	/*
	* angle in radians
	* copy glm::rotate but with glm::mat3
	*/
	static glm::mat3 rotateMatrix3(glm::mat3 const& m, float angle, glm::vec3 const& v);

	static glm::mat3  ScaleMatrix3(glm::mat3 const& m, glm::vec3 const& v);
	/**
	* Converts degrees to radians.
	* @param	DegVal			Value in degrees.
	* @return					Value in radians.
	*/
	template<class T>
	static inline T DegreesToRadians(T const& DegVal) 
	{
		return DegVal * (PI / 180.0f);
	}

	static double DegreesToRadians(double const& DegVal) { return DegVal * (DOUBLE_PI / 180.0); }

	/**
	* Converts radians to degrees.
	* @param	RadVal			Value in radians.
	* @return					Value in degrees.
	*/
	template<class T>
	static inline T RadiansToDegrees(T const& RadVal)
	{
		return RadVal * (180.f / PI);
	}

	static inline double RadiansToDegrees(double const& RadVal) { return RadVal * (180.0 / DOUBLE_PI); }

	static glm::mat4 GetDefaultModelMatrix()
	{
		glm::mat4 dmm = glm::mat4(0);
		dmm[0][0] = 1;// set scale
		dmm[1][1] = 1;
		dmm[2][2] = 1;
		return dmm;
	}

	// from https://gist.github.com/Reedbeta/e8d3817e3f64bba7104b8fafd62906df
	static glm::vec3 srgb_to_linear(glm::vec3 rgb);	
};