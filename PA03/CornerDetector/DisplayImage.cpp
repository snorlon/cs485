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
    //stuff we'll need for the harris corner detector 
    double sigma1 = 1.5;
    double curlyY = 0.7;
    double sigmaD = curlyY * sigma1;
    double alpha = 0.06;
    int gaussianSize = 5;
    int gaussianSize2 = 5;

    string filePaths = "Data/Test/Test1.pgm";;

    if ( argc >= 2 )
    {
        //allow user to indicate an image to view out of the set
        filePaths = argv[1];
    }

    Mat imageTemp;

    const int count = 20;

    //{left eye, right eye, nose, mouth}
    Mat images;


    //generate the file paths
    cout<<filePaths<<endl;


    //load the image
    images = imread( filePaths, 1 );

    if ( !images.data )
    {
        cout<<"Failed "<<filePaths<<endl;
        return -1;
    }
    cv::cvtColor(images, imageTemp, cv::COLOR_BGR2GRAY);
    images = imageTemp.clone();

    int height = images.rows;
    int width = images.cols;

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

    double** Ix = new double*[images.rows];
    double** Iy = new double*[images.rows];
    double** FxFx = new double*[images.rows];
    double** FxFy = new double*[images.rows];
    double** FyFy = new double*[images.rows];
    double** FxFxT = new double*[images.rows];
    double** FxFyT = new double*[images.rows];
    double** FyFyT = new double*[images.rows];
    double** FxFx2 = new double*[images.rows];
    double** FxFy2 = new double*[images.rows];
    double** FyFy2 = new double*[images.rows];
    double** R = new double*[images.rows];//for cornerness

    for(int i=0; i<images.rows;i++)
    {
        Ix[i] = new double[images.cols];
        Iy[i] = new double[images.cols];
        FxFx[i] = new double[images.cols];
        FxFy[i] = new double[images.cols];
        FyFy[i] = new double[images.cols];
        FxFxT[i] = new double[images.cols];
        FxFyT[i] = new double[images.cols];
        FyFyT[i] = new double[images.cols];
        FxFx2[i] = new double[images.cols];
        FxFy2[i] = new double[images.cols];
        FyFy2[i] = new double[images.cols];
        R[i] = new double[images.cols];

        for(int j=0; j<images.cols; j++)
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
            R[i][j] = 0;
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
                        Ix[j][i] += ((double) images.at<uchar>(j,i+x+shiftLeft)) * gaussianDerivativeX[x];
                    if(j+x+shiftLeft < height && j+x+shiftLeft >= 0)
                        Iy[j][i] += ((double) images.at<uchar>(j+x+shiftLeft,i)) * gaussianDerivativeY[x];
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
            R[i][j] = determinant - (alpha*(trace*trace));
//cout<<R[i][j]<<endl;
        }
    }

    double maxR = 0;
    //clear temp image
    for(int i=0; i<width; i++)
    {
        for(int j=0; j<height; j++)
        {
            if(R[j][i] > maxR)
                maxR = R[j][i];
        }
    }

    cout<<endl<<endl<<maxR<<endl;

    //suppress non-maxima

    //map pixels to new spots on temp image

    //clear temp image
    for(int i=0; i<width; i++)
    {
        for(int j=0; j<height; j++)
        {
            //convert pixels coords in original image
            bool isMaxima = true;//check in 3x3 neighborhood if we are the local maxima

            for(int y=0; y<3; y++)
            {
                for(int x=0; x<3; x++)
                {
                    if(j-1+y >= 0 && j-1+y < height)
                    {
                        if(i-1+x >= 0 && i-1+x < width)
                        {
                            if(x != i || y != j)
                            {
                                if(R[j-1+y][i-1+x] > R[j][i])
                                {
                                    isMaxima = false;
                                }
                            }
                        }
                    }
                }
            }

            if(R[j][i] >= 0.01*maxR && isMaxima)
                images.at<uchar>(j,i) = R[j][i];

            //circle the point
            if(R[j][i] >= 0.01*maxR && isMaxima)
                circle(images, Point(i,j), 5, Scalar( 255 ), 1);
        }
    }
    


    cout<<endl<<"Displaying image: "<<filePaths<<endl;

    namedWindow("Display Image", CV_WINDOW_AUTOSIZE );
    imshow("Display Image", images);

    waitKey(0);
    waitKey(0);
    waitKey(0);

    return 0;
}
