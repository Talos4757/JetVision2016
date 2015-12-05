/*
 * Utils.h
 *
 *  Created on: Dec 5, 2015
 *      Author: ubuntu
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <stdio.h>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

class Utils
{
public:
	static bool SafeVidCapOpen(VideoCapture& videoCapture, const string& target);
	static Point& GetCenterOfMat(Mat& mat);
};

struct UpdaterStruct
{
    VideoCapture *vidCap;
    Mat *frame;
    pthread_mutex_t *frameLocker;
    string* streamAddress;
};

#endif /* UTILS_H_ */
