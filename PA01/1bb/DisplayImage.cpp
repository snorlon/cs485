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

int main(int argc, char** argv )
{
    /*if ( argc != 2 )
    {
        printf("usage: DisplayImage <Image_Path>\n");
        return -1;
    }

    Mat image;
    image = imread( argv[1], 1 );

    if ( !image.data )
    {
        printf("No image data \n");
        return -1;
    }

    namedWindow("Display Image", CV_WINDOW_AUTOSIZE );
    imshow("Display Image", image);

    //cout<<"M = "<<image<<endl;

    waitKey(0);*/

    //process the file
    ifstream input;
    input.open("resources/Rect_128.txt");
    float value = 0.0f;
    int count = -1;
    //count how many items there are
    while(input.good())
    {
        input>>value;
        count++;
    }
    input.close();

    cout<<"Counted: "<<count<<" 1D elements"<<endl;


    float* array = new float[count];//rectangle data

    int sigma = 5;
    int maskSize = sigma*5;


    input.open("resources/Rect_128.txt");
    int index = 0;
    input>>value;
    //load in the values this time!
    while(input.good())
    {
        array[index] = value;

        index++;

        input>>value;
    }
    input.close();

    //print out gaussian info
    cout<<"Sigma: "<<sigma<<" Mask Size: "<<maskSize<<endl;

    //turn it into a 1D mat array
    //Mat data1D = Mat(1, count, CV_32F, array).clone();

    print_arr(array, count);

    //copy data into a mask
    float* gaussian_mask = new float[maskSize];
    for(int i=0; i<maskSize; i++)
    {
        gaussian_mask[i] = 0.0;
    }

    //init result array
    float* blurred_array = new float[count];
    float* blurred_array2 = new float[count];
    for(int i=0; i<count; i++)
    {
        blurred_array[i] = 0.0f;
        blurred_array2[i] = 0.0f;
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
    for(int i=0; i<count; i++)
    {
        //for each element in the mask
        for(int j=0; j<count; j++)
        {
            int currMaskIndex = maskIndex - i;
            if(currMaskIndex + j>=0 && currMaskIndex + j < maskSize)
            {
               blurred_array[i] += (array[j] * gaussian_mask[currMaskIndex + j]);
            }
        }
    }

    for(int i=0; i<count; i++)
    {
        //for each element in the mask
        for(int j=0; j<count; j++)
        {
            int currMaskIndex = maskIndex - i;
            if(currMaskIndex + j>=0 && currMaskIndex + j < maskSize)
            {
               blurred_array2[i] += (blurred_array[j] * gaussian_mask[currMaskIndex + j]);
            }
        }
    }

    cout<<endl<<"After smoothing... "<<endl;
    print_arr(blurred_array2, count);

    return 0;
}
