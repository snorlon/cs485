#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>

using namespace std;
using namespace cv;

int main(int argc, char** argv )
{
    //load the image for sizes
    string imageFileName = "data/cap01.bmp";
    Mat image = imread(imageFileName.c_str(), CV_LOAD_IMAGE_COLOR);


    vector<vector<Point2f> > final2DPoints;
    vector<vector<Point3f> > final3DPoints;

    //add in points for all 15 views
    for(int i=1; i<=15; i++)
    {
        char values[3];
        sprintf(values,"%d",i);

        string pixelPath = "data/pixel.txt";
        pixelPath.insert(10,values); 
        if(i<10)
        {
            pixelPath.insert(10,"0"); 
        }

        //get the 2d image coordinates for features
        vector<Point2f> input2DPoints;
        //read in the data for it
        ifstream i2dstream(pixelPath.c_str());//load just one file for now

        // get point from the first view
        string line;

        while(getline(i2dstream, line))
        {
            stringstream stream(line);
            for(int i=0; i<12; i++)
            {
                float x = 0;
                float y = 0;
                stream >> x >> y;
                input2DPoints.push_back(Point2f(x,y));
            }
        }

        i2dstream.close();

        //add it to the end data vector as a full view
        final2DPoints.push_back(input2DPoints);

        string worldPath = "data/world.txt";
        worldPath.insert(10,values); 
        if(i<10)
        {
            worldPath.insert(10,"0"); 
        }

        //get the 3d world coordinates for features
        vector<Point3f> input3DPoints;
        //read in the data for it
        ifstream i3dstream(worldPath.c_str());//load just one file for now

        while(getline(i3dstream, line))
        {
            stringstream stream(line);
            for(int i=0; i<12; i++)
            {
                float x = 0;
                float y = 0;
                float z = 0;
                stream >> x >> y;
                input3DPoints.push_back(Point3f(x,y,z));
            }
        }
        i3dstream.close();

        //add it to the end data vector as a full view
        final3DPoints.push_back(input3DPoints);

    }


cout<<final2DPoints.size()<<endl;
cout<<final3DPoints.size()<<endl;


    Mat intrinsic_Matrix;
                
    Mat distortion_coeffs;
    vector<Mat> rvecs;
    vector<Mat> tvecs;
    //calibrateCamera(final3DPoints, final2DPoints, image.size(), cameraMatrix, distCoeffs, rvecs, tvecs);
    calibrateCamera(final3DPoints, final2DPoints, Size(image.rows, image.cols), intrinsic_Matrix, distortion_coeffs, rvecs, tvecs);

    Mat imagePoints[15];

    ofstream output("errors.txt");

    for(int i=0; i<15; i++)
        projectPoints(final3DPoints[i], rvecs[i], tvecs[i], intrinsic_Matrix, distortion_coeffs, imagePoints[i]);

    //format file
    output<<"Images"<<"\t";
    for(int j=0; j<final2DPoints[0].size(); j++)
    {
        output<<"Error "<<(j+1)<<"\t";
    }
    output<<"Average Error\t";
    output<<endl;

    //calculate the error for each of the points in each image
    for(int i=0; i<15; i++)
    {
        double total = 0;

        output<<"Image "<<i<<"\t";
        for(int j=0; j<final2DPoints[i].size(); j++)
        {
            double error = (double) sqrt(pow(imagePoints[i].at<float>(j,0) - final2DPoints[i][j].x,2) + pow(imagePoints[i].at<float>(j,1) - final2DPoints[i][j].y,2));
            output<<error<<"\t";
            total+=error;
        }

        total/=96;
        output<<total<<"\t";

        output<<endl;
    }

    output.close();

    cout<<intrinsic_Matrix<<endl;
    cout<<distortion_coeffs<<endl;

    return 0;
}
