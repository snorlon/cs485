#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <iomanip>

#include "Gauss_masks.c"

using namespace std;
using namespace cv;

void print_arr(double** array, int width, int startIndex = 0)
{      
    for(int j=startIndex; j<width; j++)
    {
        for(int i=startIndex; i<width; i++)
        {
            cout << array[j][i];

            if(i < width - 1)
                cout << ", ";
        }
        
        cout<<endl;
    }

    cout<<endl;
}

int main(int argc, char** argv )
{
    if ( argc != 2 )
    {
        printf("usage: DisplayImage <Image_Path>\n");
        return -1;
    }

    Mat image;
    Mat imageOriginal;

    image = imread( argv[1], 1 );
    cv::cvtColor(image, imageOriginal, cv::COLOR_BGR2GRAY);
    image = imageOriginal.clone();

    int width = image.cols;
    int height = image.rows;

    cout<<"Image dimensions: "<<width<<"w "<<height<<"h"<<endl;

    if ( !image.data )
    {
        printf("No image data \n");
        return -1;
    }


    //clear our original image to fit new one
    double** gaussian_result = new double*[height];
    for(int i=0; i<height; i++)
    {
        gaussian_result[i] = new double[width];
        for(int j=0; j<width; j++)
            gaussian_result[i][j] = 0.0;
    }


    int sigma = 11;
    int maskSize = sigma*5;

    //init mask
    double** gaussian_mask = new double*[maskSize];
    for(int i=0; i<maskSize; i++)
    {
        gaussian_mask[i] = new double[maskSize];
        for(int j=0; j<maskSize; j++)
            gaussian_mask[i][j] = 0.0;
    }

    //create our mask
    //sigma and width of data
    Gauss(sigma, maskSize, gaussian_mask);

    //cout<<endl<<"Mask: "<<endl;
    //print_arr(gaussian_mask, maskSize);

    int maskIndex = (maskSize/2);

    //apply mask
    //multiply each location with the shifting gaussian
    //for each element in the blurred array, sum up all values around
    for(int k=0; k<height; k++)
    {
        for(int i=0; i<width; i++)
        {
            int currMaskIndexY = maskIndex - k;
            int currMaskIndexX = maskIndex - i;

            for(int j=0; j<height; j++)
            {
                if(currMaskIndexY + j>=0 && currMaskIndexY + j < maskSize)
                {
                    for(int m=0; m<width; m++)
                    {
                        if(currMaskIndexX + m>=0 && currMaskIndexX + m < maskSize)
                        {
                           gaussian_result[k][i] += ((imageOriginal.at<uchar>(j,m)/255.0) * gaussian_mask[currMaskIndexY + j][currMaskIndexX + m]);
                        }
                    }
                }
            }
        }
    }

    for(int i=0; i<height; i++)
    {
        for(int j=0; j<width; j++)
            image.at<uchar>(i,j) = (uchar) (gaussian_result[i][j] * 255);
    }


    namedWindow("Display Image", CV_WINDOW_AUTOSIZE );
    imshow("Display Image", image);

    waitKey(0);

    return 0;
}
