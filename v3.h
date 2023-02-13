#pragma once
struct v3 {
	float x, y, z;

	v3(float _x, float _y, float _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}

	static float len(v3 vec)
	{
		return sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
	}

	static v3 cross(v3 vec1, v3 vec2)
	{
		return v3(vec1.y*vec2.z - vec1.z*vec2.y, vec1.z*vec2.x - vec1.x*vec2.z, vec1.x*vec2.y - vec1.y*vec2.x);
	}

	v3 operator -(v3 vec)
	{
		return v3(x - vec.x, y - vec.y, z - vec.z);
	}

	v3 operator /(float a)
	{
		return v3(x / a, y / a, z / a);
	}

	static v3 normalize(v3 vec)
	{
		return vec / v3::len(vec);
	}
};