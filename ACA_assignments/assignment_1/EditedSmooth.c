#include <stdio.h>

#define N_SAMPLES	10
#define N_COEFFS	 3

double	sample[N_SAMPLES] = {1, 2, 1, 2, 1, 1, 2, 1, 2, 1};
double	coeff[N_COEFFS]=    {-0.5, 1, 0.5};
double	result[N_SAMPLES];

int i; // globally declare i

void smooth() // remove arguments
{
  // remove int j

	double norm;

	norm  = coeff[0]>0 ? coeff[0] : -coeff[0]; 
        norm += coeff[1]>0 ? coeff[1] : -coeff[1];
        norm += coeff[2]>0 ? coeff[2] : -coeff[2];                             

// Unroll the loop
result[0] = sample[0]; 
result[N_SAMPLES-1] = sample[N_SAMPLES-1]; 

	for (i=1; i<=N_SAMPLES-2; i++)
{    
			
// remove 2   iterations from the for loop
			
                result[i] = sample[i-1]*coeff[0];
		result[i] += sample[i]*coeff[1];
                result[i] += sample[i+1]*coeff[2];
		result[i]/=norm;

// remove j for-loop
// remove if else case for i==0 || i==n-1
	
}

}

int main(int argc, char *arvg[])
{

	if (N_SAMPLES>=N_COEFFS)

{		smooth(); // remove arguments

	for (i=0; i<N_SAMPLES; i++)
		printf("%f\n", result[i]); //only for debugging
}

}

