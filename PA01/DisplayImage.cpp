#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <iomanip>

#include "blur_tools.cpp"

using namespace std;
using namespace cv;

int main(int argc, char** argv )
{
    if ( argc != 2 )
    {
        printf("./DisplayImage <Image_Path>\n");
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

    float sigma = 2;

    //blur the image first
    blurMat(image, sigma, sigma*5);

    //SOBEL MASK STUFF
    

    //clear our original image to fit new one
    double** sobel_result = new double*[height];
    for(int i=0; i<height; i++)
    {
        sobel_result[i] = new double[width];
        for(int j=0; j<width; j++)
            sobel_result[i][j] = 0.0;
    }


    //init mask
    double sobel_mask_dx[3][3] = {{-1,0,1},{-2,0,2},{-1,0,1}};
    double sobel_mask_dy[3][3] = {{-1,-2,-1},{0,0,0},{1,2,1}};

    int maskIndex = 1;
    int maskSize = 3;

    for(int i=0; i<height; i++)
    {
        for(int j=0; j<width; j++)
        {
            //hit all elements in the sobel mask for this pixel
            for(int m=-1; m<2; m++)
            {
                for(int n=-1; n<2; n++)
                {
                    if(i+m > 0 && i+m < height && j+n>0 && j+n<width)
                        sobel_result[i][j] += ((imageOriginal.at<uchar>(i + m,j + n)/255.0) * sobel_mask_dy[m+1][n+1]);
                }
            }
        }
    }

    for(int i=0; i<height; i++)
    {
        for(int j=0; j<width; j++)
            image.at<uchar>(i,j) = ((uchar) std::max(0.0, std::min((sobel_result[i][j] * 255), 255.0)));
    }


    //DONE



    namedWindow("Display Image", CV_WINDOW_AUTOSIZE );
    imshow("Display Image", image);

    waitKey(0);

    return 0;
}
