/* **************************** */
/* First derivative of Gaussian */
/* **************************** */

#define pi 3.141592653589793

//s sigma
//hSize array size
//h storage for derivative of gaussian

void Gauss_deriv1 (double s, int Hsize, double * H)
{
  int     i;
  double  x,cst,tssq;

  cst = 1.0/(s*s*s*sqrt(2.0*pi)) ;
  tssq = -1.0/(2.0*s*s) ;

  for (i=0; i<Hsize; i++) 
  {
    x = (float)(i-Hsize/2);
    H[i] = -x*cst*exp(x*x*tssq);
  }
}

#include <math.h>

#define pi 3.141592653589793



/* Create a one-dimensional Gaussian mask
 "mask" of scale "sigma" (sigma) on "mask_size" pixels.

   The values of the mask are scaled so that the
 sum of mask values = 1.0 
*/
 


void Gauss (float sigma, int mask_size, double * mask)

{
	int i;
	double sqrt2PiSigmaSq = 1.0f / (sigma * sqrt(2.0 * pi));
    double exponentConst = -1.0f / (2 * sigma * sigma);
    double x;
    double sum;
  
	for(i = 0; i < mask_size; i++) 
	{
        //determine position relative to center of mask
		x = (double)(i - mask_size / 2);
    
        //
		mask[i] = (sqrt2PiSigmaSq * exp(x * x * exponentConst));
	}
  	
    //normalize the mask values
	sum=0.0;
  
	for(i = 0; i < mask_size; i++)
		sum += mask[i];
  
	for(i = 0; i < mask_size; i++)
        mask[i] /= sum;
}
