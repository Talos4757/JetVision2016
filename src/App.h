/*
 * App.h
 *
 *  Created on: Dec 5, 2015
 *      Author: ubuntu
 */

#ifndef APP_H_
#define APP_H_

#include <stdio.h>
#include <ctime>
#include <vector>
#include <pthread.h>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/cudafilters.hpp>
#include <opencv2/cudaarithm.hpp>
#include <opencv2/cudaimgproc.hpp>

#include "Utils.h"

#define MIN_THRESH 180
#define MAX_THRESH 255

#define USED_CLOCK CLOCK_MONOTONIC_RAW
#define NANOS 1000000000LL
#define NRATIO 1000000000.0

using namespace std;
using namespace cv;

class App
{
public:
	App(bool& display, bool& debugDisplay);
	~App();
	int Run();

private:
	static void* ReadFrameAsync(void* arg);
	Mat& PrepareFrame(Mat& src);
	Mat& FindTargets(Mat& src);

	bool *display, *debugDisplay;
	string *videoStreamAddress;
	Mat *dummyMat, *targetDraw, *criticalFrame;
	cuda::GpuMat *raw, *channels, *dst;
	VideoCapture *videoCapture;

	pthread_mutex_t *frameLocker;
	pthread_t *updaterThread;
};

#endif /* APP_H_ */
