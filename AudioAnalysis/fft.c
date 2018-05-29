#include "fft.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>


cmplx __cmplx (double real, double imag)
{
	cmplx result;
	result.real = real;
	result.imag = imag;
	return result;
}

void separate (cmplx* ptr, int size) {
	cmplx * tmp = (cmplx*)malloc (sizeof (cmplx) * size / 2);

	int i = 0;
	for (i = 0; i < size / 2; i++)
		tmp[i] = ptr[i * 2 + 1];

	i = 0;
	for (i = 0; i < size / 2; i++)
		ptr[i] = ptr[i * 2];

	i = 0;
	for (i = 0; i < size / 2; i++)
		ptr[i + size / 2] = tmp[i];

	free (tmp);
}

cmplx cmplx_add (cmplx* ptr1, cmplx*ptr2)
{
	return __cmplx (ptr1->real + ptr2->real, ptr1->imag + ptr2->imag);
}

cmplx cmplx_sub (cmplx* ptr1, cmplx*ptr2)
{
	return __cmplx (ptr1->real - ptr2->real, ptr1->imag - ptr2->imag);
}

cmplx cmplx_mult (cmplx* ptr1, cmplx* ptr2)
{
	double real = ptr1->real * ptr2->real - ptr1->imag * ptr2->imag;
	double imag = ptr1->real * ptr2->imag + ptr1->imag * ptr2->real;

	return __cmplx (real, imag);
}

cmplx cmplx_exp (cmplx* ptr)
{
	double real = ptr->real;
	double imag = ptr->imag;

	double angle = atan (imag / real);
	double mag = exp (real);

	double cosine = cos (imag);
	double sine = sin (imag);

	cmplx result;
	result.real = cosine * mag;
	result.imag = sine * mag;

	return result;
}

void fft (cmplx * ptr, int size)
{
	if (size < 2)
	{
		// End of recursion
	}
	else
	{
		separate (ptr, size);
		fft (ptr, size / 2);
		fft (ptr + size / 2, size / 2);

		int i = 0;
		for (i = 0; i < size / 2;i++)
		{
			cmplx n1 = ptr[i];
			cmplx n2 = ptr[i + size / 2];

			cmplx temp = __cmplx (0, -2. * PI * i / size);

			cmplx w = cmplx_exp (&temp);

			cmplx mult_result = cmplx_mult (&n2, &w);

			ptr[i] = cmplx_add (&n1, &mult_result);
			ptr[i + size / 2] = cmplx_sub (&n1, &mult_result);
		}
	}
}

void print (cmplx* cmplx_ptr)
{
	printf ("%8.8f + %8.8f\n", cmplx_ptr->real, cmplx_ptr->imag);
}

void print_cmplx_arr (cmplx* cmplx_ptr, int n)
{
	int i;
	for (i = 0; i < n; i++)
	{
		print (&(cmplx_ptr[i]));
	}
}

int main (int args, char** argv)
{
	const int nSamples = 64;
	double nSeconds = 1.0;                      // total time for sampling
	double sampleRate = nSamples / nSeconds;    // n Hz = n / second 
	double freqResolution = sampleRate / nSamples; // freq step in FFT result
	cmplx* x = (cmplx*)malloc (sizeof (cmplx) * nSamples);                // storage for sample data
	cmplx* X = (cmplx*)malloc (sizeof (cmplx) * nSamples);        // storage for FFT answer
	const int nFreqs = 6;
	double freq[] = { 2, 5, 11, 12, 17, 29 }; // known freqs for testing

	// generate samples for testing
	int i = 0;
	for (i = 0; i < nSamples; i++) {
		cmplx tmp;
		tmp.real = 0.0;
		tmp.imag = 0.0;
		x[i] = tmp;

		// sum several known sinusoids into x[]
		int j = 0;
		for (j = 0; j < nFreqs; j++)
			x[i].real += sin (2. * PI * freq[j] * i / nSamples);
		X[i] = x[i];        // copy into X[] for FFT work & result
	}
	// compute fft for this data
	fft (X, nSamples);

	printf ("  n\tx[]\tX[]\tf\n");       // header line

	for (i = 0; i < nSamples; i++) {
		printf ("% 3d\t%+.3f\t%+.3f\t%g\n",
				i, (x[i]).real, X[i].imag, i*freqResolution);
	}



	free (x);
	free (X);
}