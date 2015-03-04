#include <opencv2/core/core.hpp> // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;

int main(int argc, const char * argv[]) {
    
    cv::Mat frame;              // Default image
    cv::Mat frame1;             // Cloned image
    cv::Mat grayscale;          // Grayscale image
    cv::Mat running_average;    // 32f, 3 channel image for running average
    cv::Mat average;            // Capture clone image
    cv::Mat blurred;            // Blurred image
    cv::Mat difference;         // Difference image
    cv::Mat thresh;             // Threshold image
    cv::Mat contours_img;       // Contours image

    vector<vector<Point> > contours;
    Scalar color = Scalar(255,105,80); // Pink bounding box color
    
    int largestArea = 0;
    int largestContourIndex = 0;
    
    int c = 100;

    // Open the default camera
    cv::VideoCapture capture(0);
    
    while(1)
    {
        // Capture an image
        capture >> frame;

        frame1 = frame.clone();
        
        // Take running average of frame
        running_average = cv::Mat(frame.size(), CV_32FC3);
        cv::accumulateWeighted(frame, running_average, 0.16);
        
        // Swap running average to same format as frame
        cv::convertScaleAbs(running_average, running_average);
        
        // Blur image
        cv::GaussianBlur(frame1, frame1, cv::Size(5,5), 1.5);
        
        // Calculate the absolute difference
        cv::absdiff(frame1, running_average, difference);
        
        // Convert image to grayscale
        // Convert 8-bit unsigned 1 channel to 3 channel grayscale
        grayscale = cv::Mat(frame.size(), CV_8UC1);
        cv::cvtColor(difference, difference, CV_BGR2GRAY);

        // Threshold grayscale
        cv::threshold(difference, difference, 0, 255, CV_THRESH_BINARY);

        // Threshold grayscale
        //cv::Canny(grayscale, grayscale, 80, 100);

        // Blur grayscale - work on this second
        cv::GaussianBlur(difference, difference, cv::Size(5,5), 1.5);

        cv::threshold(difference, difference, 250, 255, CV_THRESH_BINARY);
        
        // Find the contours
        cv::findContours(difference, contours, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
        
        vector<Rect> boundingRects(contours.size());

        cv::Mat contourImage(difference.size(), CV_8UC3, cv::Scalar(0,0,0));
        
        for (int i = 0; i < contours.size(); i++) {
            
            // Find the area of the contour
            double area = contourArea(contours[i], false);
            
            // Used in finding the bounds of the rectangle
            boundingRects[i] = boundingRect(Mat(contours[i]));
            
            // Draw the contours
            cv::drawContours(contourImage, contours, i, color, CV_FILLED);
            
            // This throws away all areas out size of this range
            if (area < 3000 || area > 1000000){}
                else {
                    largestArea = area;
                // store the location of the motion for creating the rectangle
                    largestContourIndex = i;
                }
                
                rectangle(frame1, boundingRects[largestContourIndex].tl(), boundingRects[largestContourIndex].br(), color, 2, 8, 0);
            }
            
        // Show three windows - thresh, tours, contours (with red boxes)
            //cv::imshow("OpenCV",frame);
            //cv::imshow("GrayScale", thresh);
            cv::imshow("window", frame1);
            cv::imshow("box", contourImage);
            
            c = cv::waitKey(10);
            if(c==27)
                break;
        }
        
        return 0;
    }

