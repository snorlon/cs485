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
    if ( argc != 3 )
    {
        printf("./DisplayImage <Image_Path> <UChar Threshold 0-255>\n");
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

    int threshold = atoi(argv[2]);

    //clear our original image to fit new one
    double** sobel_result_x = new double*[height];
    double** sobel_result_y = new double*[height];
    double** sobel_gradient_magnitude = new double*[height];
    double** sobel_gradient_direction = new double*[height];
    for(int i=0; i<height; i++)
    {
        sobel_result_x[i] = new double[width];
        sobel_result_y[i] = new double[width];
        sobel_gradient_magnitude[i] = new double[width];
        sobel_gradient_direction[i] = new double[width];
        for(int j=0; j<width; j++)
        {
            sobel_result_x[i][j] = 0.0;
            sobel_result_y[i][j] = 0.0;
            sobel_gradient_magnitude[i][j] = 0.0;
            sobel_gradient_direction[i][j] = 0.0;
        }
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
                    {
                        sobel_result_x[i][j] += ((imageOriginal.at<uchar>(i + m,j + n)/255.0) * sobel_mask_dx[m+1][n+1]);
                        sobel_result_y[i][j] += ((imageOriginal.at<uchar>(i + m,j + n)/255.0) * sobel_mask_dy[m+1][n+1]);
                    }
                }
            }
            int sobel_x = (sobel_result_x[i][j] * 255);
            int sobel_y = (sobel_result_y[i][j] * 255);

            sobel_gradient_magnitude[i][j] = std::max(0.0, std::min(sqrt((sobel_x * sobel_x) + (sobel_y * sobel_y)), 255.0));
            sobel_gradient_direction[i][j] = atan2(sobel_x,sobel_y);
        }
    }

    for(int i=0; i<height; i++)
    {
        for(int j=0; j<width; j++)
        {
            int sobel_x = (sobel_result_x[i][j] * 255);
            int sobel_y = (sobel_result_y[i][j] * 255);

            if(sobel_gradient_magnitude[i][j] > threshold)
                image.at<uchar>(i,j) = 0;
            else
                image.at<uchar>(i,j) = 255;
            //image.at<uchar>(i,j) = ((uchar) std::max(0.0, std::min((sobel_result[i][j] * 255), 255.0)));
        }
    }


    //DONE



    namedWindow("Display Image", CV_WINDOW_AUTOSIZE );
    imshow("Display Image", image);

    waitKey(0);

    return 0;
}
