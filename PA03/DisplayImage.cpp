#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdlib.h>
#include <math.h>
#include <string>

#include "solve_system.cpp"
#include "FirstDerivativeGaussian.c"

using namespace std;
using namespace cv;

int main(int argc, char** argv )
{
    //for the harris-laplace
    int n = 17;
    double k = 1.2;
    double sigma0 = 1.5;

    //stuff we'll need for the harris corner detector 
    double curlyY = 0.7;
    double alpha = 0.06;
    int gaussianSize = 5;
    int gaussianSize2 = 5;
    int imgIndex = 0;

    string filePaths = "Data/Test/Test1.pgm";;

    if ( argc >= 3 )
    {
        //allow user to indicate an image to view out of the set
        filePaths = argv[1];

        //allow selecting the scale to view too
        imgIndex = atoi(argv[2]);
    }

    Mat imageTemp;

    const int count = 20;

    Mat originalImage;
    Mat scaledImages[n];


    //generate the file paths
    cout<<filePaths<<endl;


    //load the image
    originalImage = imread( filePaths, 1 );

    if ( !originalImage.data )
    {
        cout<<"Failed "<<filePaths<<endl;
        return -1;
    }
    cv::cvtColor(originalImage, originalImage, cv::COLOR_BGR2GRAY);

    const int height = originalImage.rows;
    const int width = originalImage.cols;

    double*** R = new double**[n];//for cornerness
    bool*** isMaxima = new bool**[n];

        
    for(int i=0; i<n;i++)
    {
        R[i] = new double*[height];
        isMaxima[i] = new bool*[height];

        for(int j=0; j<height; j++)
        {
            R[i][j] = new double[width];
            isMaxima[i][j] = new bool[width];

            for(int k=0; k<width; k++)
            {
                R[i][j][k] = 0;
                isMaxima[i][j][k] = false;
            }
        }
    }

    
    double maxR[n];
    for(int i=0; i<n; i++)
        maxR[i] = 0;

    //calculate the interest points for 0 to n-1
    for(int m=0; m<n; m++)
    {
        //give us a copy of the image
        scaledImages[m] = originalImage.clone();

        //parameters this iteration
        double sigma1 = pow(k,m)*sigma0;
        double sigmaD = curlyY * sigma1;

        double gaussianDerivativeX[gaussianSize];
        double gaussianDerivativeY[gaussianSize];
        double gaussianDerivativeT[gaussianSize2];

        for(int i=0; i<gaussianSize; i++)
            gaussianDerivativeX[i] = 0.0f;
        for(int i=0; i<gaussianSize; i++)
            gaussianDerivativeY[i] = 0.0f;
        for(int i=0; i<gaussianSize2; i++)
            gaussianDerivativeT[i] = 0.0f;

        //create a vertical gaussian
        Gauss_deriv1(sigmaD, gaussianSize, gaussianDerivativeY);
        //create a horizontal gaussian
        Gauss_deriv1(sigmaD, gaussianSize, gaussianDerivativeX);

        //Gauss(sigma1, gaussianSize2, gaussianDerivativeT);
        Gauss_deriv1(sigma1, gaussianSize2, gaussianDerivativeT);

        double** Ix = new double*[height];
        double** Iy = new double*[height];
        double** FxFx = new double*[height];
        double** FxFy = new double*[height];
        double** FyFy = new double*[height];
        double** FxFxT = new double*[height];
        double** FxFyT = new double*[height];
        double** FyFyT = new double*[height];
        double** FxFx2 = new double*[height];
        double** FxFy2 = new double*[height];
        double** FyFy2 = new double*[height];

        for(int i=0; i<height;i++)
        {
            Ix[i] = new double[width];
            Iy[i] = new double[width];
            FxFx[i] = new double[width];
            FxFy[i] = new double[width];
            FyFy[i] = new double[width];
            FxFxT[i] = new double[width];
            FxFyT[i] = new double[width];
            FyFyT[i] = new double[width];
            FxFx2[i] = new double[width];
            FxFy2[i] = new double[width];
            FyFy2[i] = new double[width];

            for(int j=0; j<width; j++)
            {
                Ix[i][j] = 0;
                Iy[i][j] = 0;
                FxFx[i][j] = 0;
                FxFy[i][j] = 0;
                FyFy[i][j] = 0;
                FxFxT[i][j] = 0;
                FxFyT[i][j] = 0;
                FyFyT[i][j] = 0;
                FxFx2[i][j] = 0;
                FxFy2[i][j] = 0;
                FyFy2[i][j] = 0;
            }
        }

        //convolve the image with the gaussians
            //find Ix
        for(int i=0; i<width; i++)
        {
            for(int j=0; j<height; j++)
            {
                //calculate our shift from center
                int shiftLeft = -(gaussianSize/2);

                //iterate across each of the gaussian for each pixel and add it to it's respective bucket
                for(int x=0; x<gaussianSize; x++)
                    {
                        if(i+x+shiftLeft < width && i+x+shiftLeft >= 0)
                            Ix[j][i] += ((double) scaledImages[m].at<uchar>(j,i+x+shiftLeft)) * gaussianDerivativeX[x];
                        if(j+x+shiftLeft < height && j+x+shiftLeft >= 0)
                            Iy[j][i] += ((double) scaledImages[m].at<uchar>(j+x+shiftLeft,i)) * gaussianDerivativeY[x];
                    }

            }
        }

        for(int i=0; i<width; i++)
        {
            for(int j=0; j<height; j++)
            {
                FxFx[j][i] = Ix[j][i] * Ix[j][i];
                FxFy[j][i] = Iy[j][i] * Ix[j][i];
                FyFy[j][i] = Iy[j][i] * Iy[j][i]; 
            }
        }

        //recalculate gaussian with sigma1

        //create a horizontal gaussian
        //Gauss_deriv1(sigma1, width, gaussianDerivative);

        //calculate FxFx2, FxFy2, FyFy2
        //convolve the image with the gaussians in x and y direction
        for(int i=0; i<height; i++)
        {
            for(int j=0; j<width; j++)
            {

                //calculate our shift from center
                int shiftLeft = -(gaussianSize2/2);

                //iterate across each of the gaussian for each pixel and add it to it's respective bucket
                for(int y=0; y<gaussianSize2; y++)//across rows
                    {
                        if(i+y+shiftLeft < height && i+y+shiftLeft >= 0)
                        {
                            for(int x=0; x<gaussianSize2; x++)//across cols
                                {
                                    if(j+x+shiftLeft < width && j+x+shiftLeft >= 0)
                                    {
                                        FxFxT[i][j] += FxFx[i+y+shiftLeft][j+x+shiftLeft] * gaussianDerivativeT[y];
                                        FxFyT[i][j] += FxFy[i+y+shiftLeft][j+x+shiftLeft] * gaussianDerivativeT[y];
                                        FyFyT[i][j] += FyFy[i+y+shiftLeft][j+x+shiftLeft] * gaussianDerivativeT[y];
                                    }
                                }
                        }
                    }
                for(int y=0; y<gaussianSize2; y++)//across rows
                    {
                        if(i+y+shiftLeft < height && i+y+shiftLeft >= 0)
                        {
                            for(int x=0; x<gaussianSize2; x++)//across cols
                                {
                                    if(j+x+shiftLeft < width && j+x+shiftLeft >= 0)
                                    {
                                        FxFx2[i][j] += FxFxT[i+y+shiftLeft][j+x+shiftLeft] * gaussianDerivativeT[x];
                                        FxFy2[i][j] += FxFyT[i+y+shiftLeft][j+x+shiftLeft] * gaussianDerivativeT[x];
                                        FyFy2[i][j] += FyFyT[i+y+shiftLeft][j+x+shiftLeft] * gaussianDerivativeT[x];
                                    }
                                }
                        }
                    }


                //calculate cornerness
                //get determinant
                double determinant = (FxFx2[i][j]*FyFy2[i][j] - FxFy2[i][j]*FxFy2[i][j]);

                //get trace
                double trace = FxFx2[i][j] + FyFy2[i][j];

                //R = Det(H) - k*(Trace(H))^2
                R[m][i][j] = determinant - (alpha*(trace*trace));
    //cout<<R[i][j]<<endl;
            }
        }

        //clear temp image
        for(int i=0; i<width; i++)
        {
            for(int j=0; j<height; j++)
            {
                if(R[m][j][i] > maxR[m])
                    maxR[m] = R[m][j][i];
            }
        }

        //cout<<endl<<endl<<maxR<<endl;

        //suppress non-maxima

        //map pixels to new spots on temp image
    }

    int interestCount = 0;    

    for(int m=0; m<n; m++)
    {
        //generate the images
        for(int i=0; i<width; i++)
        {
            for(int j=0; j<height; j++)
            {
                //convert pixels coords in original image
                //check in 3x3 neighborhood if we are the local maxima

                isMaxima[m][j][i] = (R[m][j][i] >= 0.1*maxR[m]);

                //check if we beat the threshold

                for(int y=0; y<3&&isMaxima[m][j][i]; y++)
                    for(int x=0; x<3&&isMaxima[m][j][i]; x++)
                        if(j-1+y >= 0 && j-1+y < height)
                            if(i-1+x >= 0 && i-1+x < width)
                                if(R[m][j-1+y][i-1+x] > R[m][j][i])
                                    isMaxima[m][j][i] = false;

                if(m>0)
                    for(int y=0; y<3&&isMaxima[m][j][i]; y++)
                        for(int x=0; x<3&&isMaxima[m][j][i]; x++)
                            if(j-1+y >= 0 && j-1+y < height)
                                if(i-1+x >= 0 && i-1+x < width)
                                    if(R[m-1][j-1+y][i-1+x]/maxR[m-1] > R[m][j][i]/maxR[m])
                                        isMaxima[m][j][i] = false;
                if(m<n-1)
                    for(int y=0; y<3&&isMaxima[m][j][i]; y++)
                        for(int x=0; x<3&&isMaxima[m][j][i]; x++)
                            if(j-1+y >= 0 && j-1+y < height)
                                if(i-1+x >= 0 && i-1+x < width)
                                    if(R[m+1][j-1+y][i-1+x]/maxR[m+1] > R[m][j][i]/maxR[m])
                                        isMaxima[m][j][i] = false;

                //check the layer above for all in the area around


                /*if(R[j][i] >= 0.01*maxR && isMaxima)
                    scaledImages[m].at<uchar>(j,i) = R[j][i];*/

                //circle the point
                if(isMaxima[m][j][i])
                {
                    circle(originalImage, Point(i,j), pow(k,m) + 3, Scalar( 255 ), 1);
                    circle(originalImage, Point(i,j), pow(k,m)*1.1 + 3, Scalar( 0 ), 1);
                    circle(scaledImages[m], Point(i,j), pow(k,m) + 3, Scalar( 255 ), 1);

                    interestCount++;
                }
            }
        }
    }

    int interestCountUnique = 0;
    for(int i=0; i<height; i++)
        for(int j=0; j<width; j++)
            for(int k=0; k<n; k++)
            {
                if(isMaxima[k][i][j])
                {
                    interestCountUnique++;
                    k=n;
                }
            }

    cout<<"There were a total of "<<interestCount<<" interest points detected among the scales."<<endl;
    cout<<"There were "<<interestCountUnique<<" UNIQUE interest points detected among the scales."<<endl;
    
    //print the scaled images with their interest points drawn
    for(int i=0; i<n; i++)
    {
        //save the faces
        imwrite(format("%s/%d.png", "interestImages", i), scaledImages[i]);
    }


    cout<<endl<<"Displaying image: "<<filePaths<<endl;

    namedWindow("Display Image", CV_WINDOW_AUTOSIZE );
    imshow("Display Image", originalImage);

    waitKey(0);
    waitKey(0);
    waitKey(0);

    return 0;
}
