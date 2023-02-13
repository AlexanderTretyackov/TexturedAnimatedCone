#include <iostream>
#include <cstdlib>
#include <math.h>
#include <vector>
#include "v3.h"
#define _USE_MATH_DEFINES
#define phi 3.1415926535
using namespace std;

const int N = 16;

class matrix
{
private:
	float m[N];

public:
	// КОНСТРУКТОРЫ

	// Пустой пконструктор (возвращает единичную матрицу)
	matrix() : matrix(1.) {}

	// Конструктор с одним параметром (число на диагонали)
	matrix(float i) : 
		m { i, 0., 0., 0., 
			0., i, 0., 0., 
			0., 0., i, 0., 
			0., 0., 0., i } {}
	
	// Конструктор с одним параметром - одномерный массив из 16 элементов.
	matrix(float m[N])
	{
		this->set_vlaue(m);
	}

	// Свойства поля m
	float* get_value()
	{
		return m;
	}

	void set_vlaue(float _m[N])
	{
		memcpy(m, _m, 16 * sizeof(float));
	}
	
	// Перегрузка оператоа *
	matrix operator*(matrix mat)
	{
		float res[N];
		for (int r = 0; r < N; r += 4) {
			for (int k = 0; k < 4; k++) {
				float sum = 0;
				for (int i = k, j = 0; i < N; i += 4, j++)
					sum += m[i] * mat.get_value()[j + r];
				res[r + k] = sum;
			}
		}
		return matrix(res);
	}

	// СТАТИЧЕСКИЕ ФУНКЦИИ 

	// Масштабирование
	static matrix scale(matrix m, v3 v)
	{
		float S[N] {
			v.x, 0., 0., 0.,
			0., v.y, 0., 0.,
			0., 0., v.z, 0.,
			0., 0., 0.,  1.
		};
		return m * S;
	}

	// Перенос
	static matrix translate(matrix m, v3 v)
	{
		float T[N] {
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			v.x, v.y, v.z, 1
		};
		return m * T;
	}
	
	// Поворот
	static matrix rotate(matrix m, float angle, v3 v)
	{
		angle = angle * phi / 180.;
		float c = cos(angle), s = sin(angle);
		float R[N] {
			v.x * v.x * (1 - c) + c,		v.x * v.y * (1 - c) + v.z * s,	v.x * v.z * (1 - c) - v.y * s,	0,
			v.y * v.x * (1 - c) - v.z * s,	v.y * v.y * (1 - c) + c,		v.y * v.z * (1 - c) + v.x * s,	0,
			v.x * v.z * (1 - c) + v.y * s,	v.y * v.z * (1 - c) - v.x * s,	v.z * v.z * (1 - c) + c,		0,
			0,								0,								0,								1
		};	
		return m * R;
	}

	// Матрица Model
	static matrix getModelMatrix()
	{
		return matrix(1);
	}

	// Матрица View
	static matrix getViewMatrix(v3 E, v3 C, v3 u)
	{
		v3 f = v3::normalize(C - E);
		v3 s = v3::normalize(v3::cross(f, u));
		v3 v = v3::cross(s, f);

		float M1[N]{
			s.x, v.x, -f.x, 0.,
			s.y, v.y, -f.y, 0.,
			s.z, v.z, -f.z, 0.,
			0.,   0.,  0.,  1.
		};
		float M2[N]{
			1.,   0.,   0.,   0.,
			0.,   1.,   0.,   0.,
			0.,   0.,   1.,   0.,
			-E.x, -E.y, -E.z, 1.
		};
		return matrix(M1) * matrix(M2);
	}

	// Матрица Projection (параллельная проекция)
	static matrix getPparMatrix(float l, float r, float b, float t, float n, float f)
	{
		float Ppar[N] =
		{
			2. / (r - l), 0., 0., 0.,
			0., 2. / (t - b), 0., 0.,
			0., 0, -2. / (f - n), 0.,
			-(r + l) / (r - l), -(t + b) / (t - b), -(f + n) / (f - n), 1.
		};

		return Ppar;
	}

	// Матрица Projection (перспективная проекция)
	static matrix getPperspMatrix(float l, float r, float b, float t, float n, float f)
	{
		float Ppersp[N] =
		{
			2. * n / (t - b), 0., 0., 0.,
			0., 2. * n / (r - l), 0., 0.,
			(r + l) / (r - l), (t + b) / (t - b), -(f + n) / (f - n), -1.,
			0., 0., -2.* f * n / (f - n), 0.
		};
		return Ppersp;
	}

	// Матрица Projection (перспективная проекция) с углом обзора
	static matrix getPperspMatrix(float n, float f, float w, float h, float teta)
	{
		float tg = tanf(teta * phi / 180./ 2.);
		return getPperspMatrix(-n * tg, n * tg, -n * w / h * tg, n * w / h * tg, n, f);
	}

	static float* getMatix3x3(matrix m)
	{
		auto _m = m.get_value();
		float m3x3[9]{
			_m[0], _m[1], _m[2],
			_m[4], _m[5], _m[6],
			_m[8], _m[9], _m[10]
		};
		return m3x3;
	}

	static float * getInvMatrixNotTranspose(matrix mat)
	{
		float* m = getMatix3x3(mat);
		float minorsMatrix[9]{
			m[4] * m[8] - m[5] * m[7], m[5] * m[6] - m[3] * m[8], m[3] * m[7] - m[6] * m[4],
			m[2] * m[7] - m[1] * m[8], m[0] * m[8] - m[2] * m[6], m[6] * m[1] - m[0] * m[7],
			m[1] * m[5] - m[4] * m[2], m[3] * m[2] - m[0] * m[5], m[0] * m[4] - m[1] * m[3],
		};

		float det = m[0] * m[4] * m[8] + m[3] * m[7] * m[2] + m[1] * m[5] * m[6]
			- m[6] * m[4] * m[2] - m[7] * m[5] * m[0] - m[1] * m[3] * m[8];

		for (int i = 0; i < 9; i++)
		{
			//minorsMatrix[i] = minorsMatrix[i] * (1 / det);
		}

		//for (int i = 0; i < 9; i++) {
		//	if (i % 3 == 0) cout << endl;
		//	cout << minorsMatrix[i] << "   ";
		//}
		return minorsMatrix;
	}
};
