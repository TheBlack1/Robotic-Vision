
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 * Motion_d.ccp                                                                    *
 * This Program detects motion and puts a red box around where the motion would be *
 * Author: Nathan Rauch                                                            *
 * Data: 2/4/2015                                                                  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* Opencv libary calls */
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp> 
#include <opencv2/imgproc/imgproc.hpp> 
 
using namespace cv;

int main()
{
  cv::Mat frame;               /* This is the image captured from the webcam */
  cv::VideoCapture capture(0); /* Capture data from the webcam */

  while(1)
     {
        int largestArea = 0;                            /* store the largest area found from the contours */
        int largestContourIndex = 0;                    /* store the location of this largest contour for creating rectangle */
        cv::Scalar colors = cv::Scalar(0,0,255);        /* create a red color for draw contours */

        capture >> frame;                               /* put camera data into frame image */
        cv::Mat image = frame.clone();                  /* create a clone of the frame image */
        cv::Mat acc = cv::Mat(frame.size(), CV_32FC3);  /* create blank image for accumulate */
        cv::Mat output = cv::Mat(frame.size(),CV_8UC3); /* create blank image for output */
        vector<vector <Point> > contours;               /* create vector for finding contours */
        vector<Vec4i> hierarchy;                        /* also used in contours */

        /* have a running average on the image and store in image acc*/
        accumulateWeighted(image,acc,0.32);
        /* convert image formate from 32FC3 to 8UC3 */
        cv::convertScaleAbs(acc,acc);
        /* gaussian blur the original image and store it in image */
        cv::GaussianBlur(image,image,cv::Size(5,5), 1.5); 
        /* find the difference between the original blured image and the running weighted image and store it in output */
        absdiff(image,acc,output);
        /* convert the output image to grayscale */
        cv::cvtColor(output, output, CV_BGR2GRAY);
        /* run image through a grey scale thresh hold */
        cv::threshold(output, output, 2, 255, CV_THRESH_BINARY);
        /* blur the greyscale output image again */
        cv::GaussianBlur(output,output,cv::Size(5,5), 1.5);
        /* run image through a grey scale thresh hold again */
        cv::threshold(output, output, 250, 255, CV_THRESH_BINARY);
        /* find the contours of the output image and output the to the vector */
        findContours(output, contours, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

        /* create a vector for the creation of a rectangle that is the size of the contour vector */
        vector<Rect> boundingRects(contours.size());

        /* creating of a blank image that is use to display the image of just the countours */
        cv::Mat contourImage(output.size(), CV_8UC3, cv::Scalar(0,0,0));
        
        /* search through vector to find contours and put a red box around the contours in motion */
        for (int i = 0; i< contours.size(); i++) {
            /* find the area of the contour */
            double area = contourArea(contours[i], false);
            /* used in finding the bounds of the red rectangle */
            boundingRects[i] = boundingRect(Mat(contours[i]));
            /* draw the contours */
            cv::drawContours(contourImage, contours, i, colors,CV_FILLED); 
            /* this if statement is to limit the range at which is used for detection and size of red rectangle */
            if(area > largestArea)
            {
              /* throw out all areas out size of this range */
              if(area < 3000 || area > 1000000){}
              else{
                largestArea = area;
                /* store the location of the motion for creation of the red rectangle */            
                largestContourIndex = i;
              }  
            }
            /* create a magic red box around each contour area in motion */
            rectangle(image, boundingRects[largestContourIndex].tl(), boundingRects[largestContourIndex].br(), colors, 2, 8, 0);
          }
         
        /* display the drawn contours */
        cv::imshow("Contours",contourImage);
        /* display the original image plus the red rectangles around the contours areas in motion in the same frame */
        cv::imshow("window", image);
        
        /* if key is pushed, end program */  
        int c = cv::waitKey(10);
          if(c==27)
            break;
        }

     return 0;
}

