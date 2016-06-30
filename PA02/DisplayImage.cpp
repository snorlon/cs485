#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdlib.h>
#include <math.h>
#include <string>

#include "solve_system.cpp"

using namespace std;
using namespace cv;

int main(int argc, char** argv )
{
    //Data for all 20 images
    int displayIndex = 0;//for the provided image to be displayed out of the set

    if ( argc >= 2 )
    {
        //allow user to indicate an image to view out of the set
        displayIndex = atoi(argv[1]) - 1;

        if(displayIndex >= 20)
            displayIndex = 19;

        if(displayIndex < 0)
            displayIndex = 0;
    }

    Mat imageTemp;

    const int count = 20;
    float preCoordinates[count][4][2]{//the points before any transformation
    {{27,51},{63,50},{45,71},{45,89}},
    {{31,44},{75,43},{67,68},{59,87}},
    {{25,48},{62,45},{46,74},{45,90}},
    {{16,45},{59,42},{29,63},{32,84}},
    {{34,44},{75,43},{71,65},{64,84}},
    {{15,47},{55,44},{26,67},{29,86}},
    {{25,44},{63,43},{44,59},{44,81}},
    {{28,46},{65,43},{49,65},{48,85}},
    {{31,37},{68,35},{54,49},{52,73}},
    {{34,52},{74,49},{64,77},{57,92}},

    {{25,52},{59,54},{40,68},{39,89}},
    {{35,51},{68,52},{55,67},{53,87}},
    {{21,51},{57,54},{33,67},{33,88}},
    {{32,54},{65,52},{50,69},{50,89}},
    {{40,53},{73,52},{60,69},{58,88}},
    {{15,52},{49,53},{25,68},{29,89}},
    {{29,52},{61,54},{45,70},{45,89}},
    {{22,51},{58,53},{36,68},{36,88}},
    {{36,52},{67,54},{51,69},{49,90}},
    {{47,54},{78,53},{72,68},{67,89}}
    };

    float postCoordinates[count][4][2]{//the points after transformation into a 48,40 area, still predetermined beforehand
    {{8,6},{39,5},{24,22},{24,36}},
    {{12,9},{40,9},{33,24},{30,35}},
    {{12,7},{36,5},{26,24},{25,36}},
    {{10,7},{41,6},{18,22},{22,36}},
    {{10,5},{41,4},{35,19},{32,34}},
    {{9,8},{39,6},{18,22},{21,35}},
    {{9,8},{38,6},{23,18},{24,34}},
    {{11,7},{38,5},{25,21},{26,35}},
    {{9,7},{37,5},{26,14},{25,32}},
    {{10,7},{39,5},{32,24},{27,35}},

    {{11,8},{36,9},{21,20},{22,34}},
    {{13,6},{36,7},{25,17},{24,31}},
    {{11,5},{39,7},{21,19},{21,34}},
    {{11,6},{38,5},{25,19},{25,35}},
    {{12,8},{37,8},{27,19},{25,33}},
    {{9,7},{34,8},{17,9},{20,34}},
    {{12,6},{35,6},{24,19},{23,32}},
    {{11,7},{38,9},{20,19},{21,34}},
    {{10,6},{37,7},{25,19},{24,35}},
    {{12,8},{37,8},{33,21},{30,36}}
    };

    //{left eye, right eye, nose, mouth}
    string filePaths[count];
    Mat images[count];


    //generate the file paths
    for(int i=0; i<count; i++)
    {
        string folder = "S1/";
        if(i >= 10)
            folder = "S2/";

        filePaths[i] = folder + to_string(((i % 10) + 1)) + ".pgm";
        cout<<filePaths[i]<<endl;


        //load the image
        images[i] = imread( filePaths[i], 1 );
        cv::cvtColor(images[i], imageTemp, cv::COLOR_BGR2GRAY);
        images[i] = imageTemp.clone();

        if ( !images[i].data )
        {
            cout<<"Failed "<<filePaths[i]<<endl;
            return -1;
        }
    }

    int m = 4;
    int n = 3;


    //generate Px, Py, and P
    float* Px = new float[m+1];
    Px[0] = 0.0f;
    for(int i=1; i<m+1; i++)
    {
        Px[i] = preCoordinates[displayIndex][i-1][0];
    }

    float* Py = new float[m+1];
    Py[0] = 0.0f;
    for(int i=1; i<m+1; i++)
    {
        Py[i] = preCoordinates[displayIndex][i-1][1];
    }


    float** P = new float*[m+1];
    for(int i=0; i<m+1; i++)
        P[i] = new float[n+1];    

    for(int i=1; i<m+1; i++)
    {
        for(int j=1; j<n; j++)
        {
            P[i][j] = postCoordinates[displayIndex][i-1][j-1];
        }

        P[i][n] = 1.0f;
    }


    /* From test.cpp*/
    int i;
    float* x = NULL;
    float* y = NULL;

    x = new float[n+1];
    for(int i=0; i<n+1; i++)
        x[i] = 0.0f;

    solve_system(m,n,P,x,Px);

    cout<<"a11: "<<x[1]<<" a12: "<<x[2]<<" b1: "<<x[3]<<endl;

    y = new float [n+1];
    for(int i=0; i<n+1; i++)
        y[i] = 0.0f;

    solve_system(m,n,P,y,Py);

    cout<<"a21: "<<y[1]<<" a22: "<<y[2]<<" b2: "<<y[3]<<endl;


    //reconstruct a and b
    float** a = new float*[2];
    for(int i=0; i<2; i++)
    {
        a[i] = new float[2];
    }

    a[0][0] = x[1];
    a[0][1] = x[2];
    a[1][0] = y[1];
    a[1][1] = y[2];

    float** b = new float*[2];
    b[0] = new float[1];
    b[1] = new float[1];

    b[0][0] = x[3];
    b[1][0] = y[3];

    //int width = image.cols;
    //int height = image.rows;

    //cout<<"Image dimensions: "<<width<<"w "<<height<<"h"<<endl;

    imageTemp = images[displayIndex].clone();
    //clear out all of the pixel values
    for(int i=0; i<imageTemp.rows; i++)
    {
        for(int j=0; j<imageTemp.cols; j++)
        {
            imageTemp.at<uchar>(j,i) = 0;
        }
    }

    //clear temp image
    for(int i=0; i<images[displayIndex].rows; i++)
    {
        for(int j=0; j<images[displayIndex].cols; j++)
        {
            //convert pixels coords in original image
            int xf = j;
            int yf = i;

            //aP + b = Pf

            xf = (int) (((a[0][0] * (float) j)+(a[0][1] * (float) i)) + b[0][0]);
            yf = (int) (((a[1][0] * (float) j)+(a[1][1] * (float) i)) + b[1][0]);

            //check pixels are within bounds
            if(xf >= images[displayIndex].cols)
                xf = images[displayIndex].cols - 1;
            if(xf < 0)
                xf = 0;
            if(yf >= images[displayIndex].rows)
                yf = images[displayIndex].rows - 1;
            if(yf < 0)
                yf = 0;

            imageTemp.at<uchar>(i,j) = images[displayIndex].at<uchar>(yf,xf);

            //if(i % 15 == 0 && j % 15 == 0)
                //cout<<xf<<"|"<<yf<<"|   |"<<j<<"|"<<i<<endl;
        }
    }

    //crop dat image
    Rect myROI(0, 0, 48, 40);
    Mat croppedImage = imageTemp(myROI);
    imageTemp = croppedImage.clone();

    cout<<endl<<endl;


    //map pixels to new spots on temp image



    cout<<endl<<"Displaying image: "<<filePaths[displayIndex]<<endl;

    namedWindow("Display Image", CV_WINDOW_AUTOSIZE );
    imshow("Display Image", imageTemp);

    waitKey(0);

    return 0;
}
