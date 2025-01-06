#pragma once

void Translate(Vector3& vector, Vector3 offset)
{
	vector = { vector.x + offset.x, vector.y + offset.y, vector.z + offset.z };
}

void Scale(Vector3& vector, Vector3 scale)
{
	vector = { vector.x * scale.x, vector.y * scale.y, vector.z * scale.z };
}

void Transform(double transform_mat[3][3], Vector3& vector)
{
	double vector_mat[3][1] = { {vector.x}, {vector.y}, {vector.z} };
	double result_mat[3][1] = { {0}, {0}, {0} };

	// matrix multiplication algorithm
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 1; j++)
			for (int k = 0; k < 3; k++)
				result_mat[i][j] += transform_mat[i][k] * vector_mat[k][j];

	vector = { result_mat[0][0], result_mat[1][0], result_mat[2][0] };
}

void Transform(double transform_mat[4][4], Vector4& vector)
{
	double vector_mat[4][1] = { {vector.x}, {vector.y}, {vector.z}, {vector.w} };
	double result_mat[4][1] = { {0}, {0}, {0}, {0} };

	// matrix multiplication algorithm
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 1; j++)
			for (int k = 0; k < 4; k++)
				result_mat[i][j] += transform_mat[i][k] * vector_mat[k][j];

	vector = { result_mat[0][0], result_mat[1][0], result_mat[2][0], result_mat[3][0] };
}

void MultiplyMatrix(double a[3][3], double b[3][3])
{
	double c[3][3];
	int i, j, k;
	for (i = 0; i < 3; ++i)
		for (j = 0; j < 3; ++j)
			c[i][j] = 0;

	for (i = 0; i < 3; ++i)
		for (j = 0; j < 3; ++j)
			for (k = 0; k < 3; ++k)
				c[i][j] += a[i][k] * b[k][j];

	for (i = 0; i < 3; ++i)
		for (j = 0; j < 3; ++j)
			a[i][j] = c[i][j];
}

void Rotate(Vector3& vector, Vector3& rotation)
{
	double rcos, rsin;


	/*while (abs(rotation.x) > PI / 2)
		rotation.x = 0;
	while (abs(rotation.y) > PI / 2)
		rotation.y = 0;
	while (abs(rotation.z) > PI / 2)
		rotation.z = 0;*/


	rcos = cos(rotation.x);
	rsin = sin(rotation.x);
	double rotateX[3][3] = { { 1,  0,      0  },
							 { 0, rcos, -rsin },
							 { 0, rsin,  rcos } };

	rcos = cos(rotation.y);
	rsin = sin(rotation.y);
	double rotateY[3][3] = { {  rcos,  0,  rsin },
							 {    0,   1,   0   },
							 { -rsin,  0,  rcos } };

	rcos = cos(rotation.z);
	rsin = sin(rotation.z);
	double rotateZ[3][3] = { { rcos, -rsin, 0 },
							 { rsin,  rcos, 0 },
							 {  0,      0,  1 } };
	

	Transform(rotateX, vector);
	Transform(rotateY, vector);
	Transform(rotateZ, vector);
}

void Normalize(Vector3& vector)
{
	double length = sqrt(pow(vector.x, 2) + pow(vector.y, 2) + pow(vector.z, 2));
	vector = { vector.x / length, vector.y / length, vector.z / length };
}

Vector3 SurfaceNormal(Vector3 first, Vector3 second, Vector3 third) // 3d perpendicular on a triangle
{
	Vector3 A = { second.x - first.x, second.y - first.y, second.z - first.z };
	Vector3 B = { third.x - first.x,  third.y - first.y,  third.z - first.z };

	return { A.y * B.z - A.z * B.y, A.z * B.x - A.x * B.z, A.x * B.y - A.y * B.x };
}

Vector3 cross(Vector3 A, Vector3 B)
{
	return { A.y * B.z - A.z * B.y, -A.x * B.z + A.z * B.x, A.x * B.y - A.y * B.x };
}

double dot(Vector3 A, Vector3 B)
{
	return A.x * B.x + A.y * B.y + A.z * B.z;
}

double Distance(Vector3 first, Vector3 second)
{
	return sqrt(pow(second.x - first.x, 2) + pow(second.y - first.y, 2) + pow(second.z - first.z, 2));
}

double Length(Vector3 vector)
{
	return Distance(vector, {0, 0, 0});
}

void Print(Vector3 vector)
{
	cout << vector.x << ' ' << vector.y << ' ' << vector.z << '\n';
}

void Print(Vector4 vector)
{
	cout << vector.x << ' ' << vector.y << ' ' << vector.z << ' ' << vector.w << '\n';
}

void Perspective(Vector3& vector)
{
	double zFar  = 100.0;
	double zNear =   0.1;

	double aspect = 1920.0/1080.0;
	double FOV    = PI/4;

	double f = 1/tan(FOV/2);

	double A = -(zFar + zNear) / (zFar - zNear);
	double B = -(2 * zFar * zNear) / (zFar - zNear);

	double perspective_mat[4][4] = { { f/aspect, 0,  0, 0 },
									 {    0,     f/aspect,  0, 0 },
									 {    0,     0,  A, B },
									 {    0,     0, -1, 0 } };

	Vector4 new_vec = { vector.x, vector.y, vector.z, 1 };
	Transform(perspective_mat, new_vec);

	// 1250 luat arbitrar daca pui un numar mai mic incep sa fie mai alungite obiectele

	// asta e partea cea mai importanta 
	// w va tine valoarea veche a lui z
	// imparti prin z varfurile. obiectele la o distanta mai mare devin mai mici cele mai apropriate devin mai mari
	vector = {vector.x/(-new_vec.w/1250+1), vector.y/(-new_vec.w/1250+1), vector.z };
	// TODO ar trebuii verificat pentru impartirea prin 0
}

void ViewMatrix(Vector3& vector)
{
	// schimbare de baza cum am facut la mate
	// vectorii nus pe coloane pentru ca asta e inversa unei alte matrici defapt
	double view_matrix[4][4] = { { camera.right.x,   camera.right.y,   camera.right.z,   -camera.position.x },
								 { camera.up.x,      camera.up.y,      camera.up.z,      -camera.position.y },
								 { camera.forward.x, camera.forward.y, camera.forward.z, -camera.position.z },
								 {        0,                0,                0,                 1          } };

	Vector4 vec = { vector.x, vector.y, vector.z, 1 };
	Transform(view_matrix, vec);
	vector = { vec.x, vec.y, vec.z };
}

bool isInScreen(Vector3& point)
{
	if (point.x >= 0 && point.x <= WINDOW_WIDTH && point.y >= 0 && point.y <= WINDOW_HEIGHT)
		return 1;
	return 0;
}
int inScreen(Vector3 &point, double m, double n)
{
	//y = mx + n - line equation
	if (isInScreen(point))
		return 1;

	point.x = 0;
	point.y = n;

	if (isInScreen(point))
		return 1;

	point.x = WINDOW_WIDTH;
	point.y = m * point.x + n;

	if (isInScreen(point))
		return 1;

	if (abs(m) > EPS)
	{
		point.y = 0;
		point.x = (-n) / m;

		if (isInScreen(point))
			return 1;
	}

	if (abs(m) > EPS)
	{
		point.y = WINDOW_HEIGHT;
		point.x = (point.y - n) / m;

		if (isInScreen(point))
			return 1;
	}

	return 0;
}
