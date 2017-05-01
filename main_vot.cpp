#include <stdlib.h>


#include <iostream>
#include <fstream>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include "src/kcf.h"
#include "vot.hpp"

#define USE_CAM 1
#define USE_FILE 0
using namespace std;
using namespace cv;
int main()
{
    //load region, images and prepare for output
    VOT vot_io("region.txt", "images.txt", "output.txt");

    KCF_Tracker tracker;
    cv::Mat image;
    //img = firts frame, initPos = initial position in the first frame
    cv::Rect init_rect = vot_io.getInitRectangle();
    vot_io.outputBoundingBox(init_rect);
#if USE_FILE
    vot_io.getNextImage(image);
#else
    VideoCapture cap;
    if (!cap.open(0))
	{
		cout << "error: could not start camera capture" << endl;
		return EXIT_FAILURE;
	}
	cout<<"camera started"<<endl;
	Mat frameOrig;
	cap >> frameOrig;
	resize(frameOrig, image, Size(320, 240));
	flip(image, image, 1);
#endif
    tracker.init(image, init_rect);
    cout<<"tracker inited"<<endl;
    BBox_c bb;
    double avg_time = 0.;
    int frames = 0;
    cvNamedWindow("Image",0);
//    image.copyTo(result);
//    cv::imshow("Image", image);
    cout<<"window opened "<<endl;
#if USE_FILE
    while (vot_io.getNextImage(image) == 1){
        double time_profile_counter = cv::getCPUTickCount();
        tracker.track(image);
        time_profile_counter = cv::getCPUTickCount() - time_profile_counter;
        //std::cout << "  -> speed : " <<  time_profile_counter/((double)cvGetTickFrequency()*1000) << "ms. per frame" << std::endl;
        avg_time += time_profile_counter/((double)cvGetTickFrequency()*1000);
        frames++;

        bb = tracker.getBBox();
        vot_io.outputBoundingBox(cv::Rect(bb.cx - bb.w/2., bb.cy - bb.h/2., bb.w, bb.h));

      //  imshow("Image",image);
        cv::rectangle(image, cv::Rect(bb.cx - bb.w/2., bb.cy - bb.h/2., bb.w, bb.h), CV_RGB(0,255,0), 2);
        cv::imshow("Image", image);
        cv::waitKey();

//        std::stringstream s;
//        std::string ss;
//        int countTmp = frames;
//        s << "imgs" << "/img" << (countTmp/10000);
//        countTmp = countTmp%10000;
//        s << (countTmp/1000);
//        countTmp = countTmp%1000;
//        s << (countTmp/100);
//        countTmp = countTmp%100;
//        s << (countTmp/10);
//        countTmp = countTmp%10;
//        s << (countTmp);
//        s << ".jpg";
//        s >> ss;
//        //set image output parameters
//        std::vector<int> compression_params;
//        compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
//        compression_params.push_back(90);
//        cv::imwrite(ss.c_str(), image, compression_params);
    }
#else
    for (int frameInd = 0; ; ++frameInd)
	{
	//	Mat frame;
		
		Mat frameOrig;
		cap >> frameOrig;
		resize(frameOrig, image, Size(320, 240));
		flip(image, image, 1);
		double time_profile_counter = cv::getCPUTickCount();
        tracker.track(image);
        time_profile_counter = cv::getCPUTickCount() - time_profile_counter;
        //std::cout << "  -> speed : " <<  time_profile_counter/((double)cvGetTickFrequency()*1000) << "ms. per frame" << std::endl;
        avg_time += time_profile_counter/((double)cvGetTickFrequency()*1000);
        frames++;

        bb = tracker.getBBox();
        vot_io.outputBoundingBox(cv::Rect(bb.cx - bb.w/2., bb.cy - bb.h/2., bb.w, bb.h));
        cv::rectangle(image, cv::Rect(bb.cx - bb.w/2., bb.cy - bb.h/2., bb.w, bb.h), CV_RGB(0,255,0), 2);
        cv::imshow("Image", image);
        cv::waitKey(1);
//        cout<<"loop"<<endl;
	}
#endif
    std::cout << "Average processing speed " << avg_time/frames <<  "ms. (" << 1./(avg_time/frames)*1000 << " fps)" << std::endl;

    return EXIT_SUCCESS;
}
