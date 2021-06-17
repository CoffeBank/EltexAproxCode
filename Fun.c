#define _CRT_SECURE_NO_WARNINGS 
#pragma warning(disable : 4996) 
#include <stdio.h> 
#include <math.h> 
#include <locale.h> 
#include <malloc.h> 
double* x, *x1, *y;
double a, b, B;
int n;

// Вычисление коэффициентов аппроксимирующей прямой по нормальному 
void getApprox(const char* k)
{
	int j;
	double sumx = 0;
	double sumy = 0;
	double sumx2 = 0;
	double sumxy = 0;
	double diff = 0;
	n = 0;
	FILE* pf = fopen(k, "r");
	while (fscanf_s(pf, "%*lg %*lg ") != EOF)
		n++;
	fseek(pf, 0, SEEK_SET);
	if (n == NULL)
	{
		free(x);
		free(y);
	}
	x = (double*)malloc(n * sizeof(double));
	x1 = (double*)malloc(n * sizeof(double));
	y = (double*)malloc(n * sizeof(double));
	for (j = 0; j < n; j++)
	{
		fscanf_s(pf, "%lg %lg", &x[j], &y[j]);
	}
	for (int i = 0; i < n; i++)
	{
		sumx += x[i];
		sumy += y[i];
		sumx2 += x[i] * x[i];
		sumxy += x[i] * y[i];
	}
	a = (n * sumxy - (sumx * sumy)) / (n * sumx2 - sumx * sumx);
	b = (sumy - a * sumx) / n;
	for (int i = 0; i < n; i++)
	{
		x1[i] = ((y[i] - b) / a);
		diff += (y[i] - (a * x[i] + b)) * (y[i] - (a * x[i] + b));
	}
	B = sqrt(diff/n);
}
