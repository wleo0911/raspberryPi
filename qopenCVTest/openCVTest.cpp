#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <string>

using namespace cv;
using namespace std;


int main(int argc, char **argv)
{
    Mat image;
    Mat convert_image;
    String file_name = "./test.png";
    image = imread(file_name, CV_WINDOW_AUTOSIZE);
	if(image.empty())
	{
		cout<<"Image is empty."<<std::end
		return -1;
	}
	
	cvtColor(image, convert_image, COLOR_BGR2GRAY)
	
    namedWindow("IMG", WINDOW_AUTOSIZE);
    imshow("IMG", image);
    
    namedWindow("IMG", WINDOW_AUTOSIZE);
    imshow("IMG", convert_image);
    
    imwrite("./conver.jpg", convert_image);
    waitKey(0);
    return 0;
}
