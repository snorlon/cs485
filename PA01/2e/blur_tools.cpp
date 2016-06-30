#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <iomanip>

#include "Gauss_masks.c"

using namespace std;
using namespace cv;

void print_arr(float array[], int width, int startIndex = 0)
{      
    for(int i=startIndex; i<width; i++)
    {
        cout << array[i];

        if(i < width - 1)
            cout << ", ";
        else
            cout << endl;
    }

    cout<<endl;
}

void blurMat(Mat image, float sigma, int maskSize)
{
    Mat imageOriginal = image.clone();

    int width = image.cols;
    int height = image.rows;

    //clear our original image to fit new one
    double** gaussian_result = new double*[height];
    for(int i=0; i<height; i++)
    {
        gaussian_result[i] = new double[width];
        for(int j=0; j<width; j++)
            gaussian_result[i][j] = 0.0;
    }


    //init mask
    float* gaussian_mask = new float[maskSize];
    for(int i=0; i<maskSize; i++)
    {
        gaussian_mask[i] = 0.0;
    }

    //create our mask
    //sigma and width of data
    Gauss(sigma, maskSize, gaussian_mask);

    cout<<endl<<"Mask: "<<endl;
    print_arr(gaussian_mask, maskSize);

    int maskIndex = (maskSize/2);

    //apply mask
    //multiply each location with the shifting gaussian
    //for each element in the blurred array, sum up all values around
    for(int k=0; k<height; k++)
    {
        for(int i=0; i<width; i++)
        {
            //for each element in the mask
            for(int j=0; j<width; j++)
            {
                int currMaskIndex = maskIndex - i;
                if(currMaskIndex + j>=0 && currMaskIndex + j < maskSize)
                {
                   gaussian_result[k][i] += ((imageOriginal.at<uchar>(k,j)/255.0) * gaussian_mask[currMaskIndex + j]);
                }
            }
        }
    }

    for(int i=0; i<height; i++)
    {
        for(int j=0; j<width; j++)
            image.at<uchar>(i,j) = (uchar) (gaussian_result[i][j] * 255);
    }

    imageOriginal = image.clone();

    

    //clear our original image to fit new one
    for(int i=0; i<height; i++)
    {
        for(int j=0; j<width; j++)
        {
            gaussian_result[i][j] = 0;
        }
    }
    

    for(int k=0; k<height; k++)
    {
        for(int i=0; i<width; i++)
        {
            //for each element in the mask
            for(int j=0; j<height; j++)
            {
                int currMaskIndex = maskIndex - k;
                if(currMaskIndex + j>=0 && currMaskIndex + j < maskSize)
                {
                   gaussian_result[k][i] += ((imageOriginal.at<uchar>(j,i)/255.0) * gaussian_mask[currMaskIndex + j]);
                }
            }
        }
    }

    for(int i=0; i<height; i++)
    {
        for(int j=0; j<width; j++)
            image.at<uchar>(i,j) = (uchar) (gaussian_result[i][j] * 255);
    }

}
