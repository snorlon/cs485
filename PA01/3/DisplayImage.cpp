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
    if ( argc != 4 )
    {
        printf("./DisplayImage <Image_Path> <σ0> <σX>\n");
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

    //GAUSSIAN PYRAMID STUFF
    int origWidth = width;
    int origHeight = height;

    int startStep = atoi(argv[2]);
    int endStep = atoi(argv[3]);

    for(int i=startStep; i<endStep; i++)
    {
        //blur at each iteration
        blurMat(image, sigma, sigma);

        origWidth = width;
        origHeight = height;

        width = width/2;
        height = height/2;

        Size newSize(width,height);

        //shrink it, averaging the newly  blurred pixels
        Mat newImage;//using shrinking function ONLY for creating a smaller mat easily, all data is overridden anyways
        resize(image,newImage,newSize);

        //map average of 4 pixels to each pixel in new image
        for(int x=0; x<width; x++)
        {
            for(int y=0; y<height; y++)
            {
                double sum = 0.0;
                int count = 0;

                //sum up the 4 pixels, less if less available
                if(x*2 < origWidth && y*2 < origHeight)
                {
                    sum += image.at<uchar>(y*2, x*2) / 255.0;
                    count++;
                }
                if(x*2 + 1 < origWidth && y*2 < origHeight)
                {
                    sum += image.at<uchar>(y*2, x*2 + 1) / 255.0;
                    count++;
                }
                if(x*2 < origWidth && y*2 + 1 < origHeight)
                {
                    sum += image.at<uchar>(y*2 + 1, x*2) / 255.0;
                    count++;
                }
                if(x*2 + 1 < origWidth && y*2 + 1 < origHeight)
                {
                    sum += image.at<uchar>(y*2 + 1, x*2 + 1) / 255.0;
                    count++;
                }

                sum /= count;

                image.at<uchar>(y,x) = (int) (sum * 255);
            }
        }

        //store it
        image = newImage;
    }

    //END



    namedWindow("Display Image", CV_WINDOW_AUTOSIZE );
    imshow("Display Image", image);

    waitKey(0);

    return 0;
}
