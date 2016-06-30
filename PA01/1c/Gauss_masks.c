#include <math.h>

#define pi 3.141592653589793



/* Create a one-dimensional Gaussian mask
 "mask" of scale "sigma" (sigma) on "mask_size" pixels.

   The values of the mask are scaled so that the
 sum of mask values = 1.0 
*/
 


void Gauss (float sigma, int mask_size, double ** mask)

{
	double Pi2SigmaSq = 1.0 / (sigma * sigma * 2.0 * pi);
    double exponentConst = -1.0 / (2 * sigma * sigma);
  
    for(int j = 0; j < mask_size; j++)
    {
	    for(int i = 0; i < mask_size; i++) 
	    {
            //determine position relative to center of mask
		    double x = (double)(i - mask_size / 2);
		    double y = (double)(j - mask_size / 2);
        
            //
		    mask[i][j] = (Pi2SigmaSq * exp(((x * x) + (y * y)) * exponentConst));
	    }
    }
  	
    //normalize the mask values  
    double sum1 = 0;

    for(int j=0; j<mask_size; j++)
    {
	    for(int i = 0; i < mask_size; i++)
        {
		    sum1 += mask[i][j];
        }
    }

    for(int j=0; j<mask_size; j++)
    {
	    for(int i = 0; i < mask_size; i++)
        {
		    mask[i][j] /= sum1;
        }
    }
}
