#ifndef __FFT__H_
#define __FFT__H_

#define PI 3.14159265358979323846
#define E 2.71828182845904523536


typedef struct tagComplex
{
	double real;
	double imag;
} cmplx;


void separate (cmplx* ptr, int size);
void fft (cmplx * ptr, int size);
cmplx __cmplx (double, double);
cmplx cmplx_exp (cmplx* ptr);
cmplx cmplx_add (cmplx* ptr1, cmplx*ptr2);
cmplx cmplx_sub (cmplx* ptr1, cmplx*ptr2);
cmplx cmplx_mult (cmplx* ptr1, cmplx*ptr2);
void print (cmplx* cmplx_ptr);


#endif
