#include <math.h>

#define pi 3.141592653589793



/* Create a one-dimensional Gaussian mask
 "mask" of scale "sigma" (sigma) on "mask_size" pixels.

   The values of the mask are scaled so that the
 sum of mask values = 1.0 
*/
 


void Gauss (float sigma, int mask_size, float * mask)

{
	int i;
	float sqrt2PiSigmaSq = 1.0f / (sigma * sqrt(2.0 * pi));
    float exponentConst = -1.0f / (2 * sigma * sigma);
    float x;
    float sum;
  
	for(i = 0; i < mask_size; i++) 
	{
        //determine position relative to center of mask
		x = (float)(i - mask_size / 2);
    
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
