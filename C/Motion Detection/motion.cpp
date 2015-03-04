/*
Author:         Karl Wilson
Professor:      Hunter Lloyd
Project:        Motion Detection OpenCV
*/

//#include <stdafx.h>
//#if !(defined (__linux__) || (defined (__APPLE__) && defined(__MACH__)))
#include <opencv2/core/core.hpp> // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/highgui/highgui.hpp> 
#include <opencv2/imgproc/imgproc.hpp> 
using namespace cv;

//int wmain(int argc, wchar_t *argv[]) //_TCHAR* argv[])
int main()
{
      cv::Mat frame;                // This is the image taken from the webcam
      int c = 100;
      cv::VideoCapture capture(0);  // This captures the data from the webcam

     while(1)
     {
              int largestArea = 0;                            // store the largest area found from the contours
              int largestContourIndex = 0;                    // store the location of this largest contour for creating the boxes
              cv::Scalar colors = cv::Scalar(180,82,15);      // the colours of the boxes

              // Puts the captured data from the webcam into the image
              capture >> frame;
              // Create a clone of the original webcam image
              cv::Mat image1 = frame.clone();
              // Create an image used for edge detection in black and white
              cv::Mat bw = cv::Mat(frame.size(), CV_8UC1);
              // Create a blank image for the accumulated weight
              cv::Mat weight = cv::Mat(frame.size(), CV_32FC3);
              //  Create a blank image for the output
              cv::Mat out = cv::Mat(frame.size(), CV_8UC3);

              vector<vector <Point> > contours;
              // vector for finding the contours

              accumulateWeighted(frame, weight, 0.5);
              // take the accumulated weight of the frame and store it in weight
              cv::convertScaleAbs(weight,weight);
              // convert image formate from 32FC3 to 8UC3
              cv::GaussianBlur(image1,image1,cv::Size(5,5), 1.5);
              // blur the original image and place it in image1
              cv::absdiff(image1,weight,out);
              // find the difference between the original blured image and the accumulate weighted image and store it in output
              cv::cvtColor(out, out, CV_BGR2GRAY);
              // convert the output image to grayscale
              cv::threshold(out, out, 6, 255, CV_THRESH_BINARY);
              // run image through a gray scale thresh hold
              cv::GaussianBlur(out,out,cv::Size(5,5), 1.5);
              // blur the grayscale output image again
              cv::threshold(out, out, 252, 255, CV_THRESH_BINARY);
              // run image through a grey scale thresh hold again
              findContours(out, contours, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
              // find the contours of the output image and output them to the vector

              vector<Rect> boundingRects(contours.size());
              // create a vector for the creation of a box that is the size of the contour vector

              cv::Mat contourImage(out.size(), CV_8UC3, cv::Scalar(0,0,0));
              // create a blank image that is use to display the image of just the countours

              for (int i = 0; i< contours.size(); i++) {
                  // find the area of the contour
                  double area = contourArea(contours[i], false);
                  // used in finding the bounds of the rectangle
                  boundingRects[i] = boundingRect(Mat(contours[i]));
                  // draw the contours
                  cv::drawContours(contourImage, contours, i, colors,CV_FILLED); 
                  
                    // throw out all areas out size of this range
                    if(area < 3000 || area > 3000000){}
                    else{
                      largestArea = area;
                      // store the location of the motion for creation of the boxes            
                      largestContourIndex = i;
                    }  
                  
                  // creates a box around each contour area in motion
                  rectangle(image1, boundingRects[largestContourIndex].tl(), boundingRects[largestContourIndex].br(), colors, 2, 1, 0);
                }

              //cv::cvtColor(frame, bw, CV_BGR2GRAY);
              //cv::Canny(bw, bw, 80, 100);
              //cv::imshow("OpenCV",frame);
              //cv::imshow("GrayScale", bw);
              //cv::imshow("Clone", out);
              cv::imshow("Contours",contourImage);
              cv::imshow("window", image1);



              c = cv::waitKey(10);
              if(c==27)
                    break;
     }

     return 0;
}
