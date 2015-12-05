/*
 * App.cpp
 *
 *  Created on: Dec 5, 2015
 *      Author: ubuntu
 */

#include "App.h"

App::App(bool display, bool debugDisplay)
{
	this->display = display;
	this->debugDisplay = debugDisplay;

	this->videoStreamAddress =  "http://root:toor@10.0.0.12/mjpg/video.mjpg";

	this->videoCapture = new VideoCapture();
	this->dummyMat = new Mat();
	this->targetDraw = new Mat();
	this->criticalFrame = new Mat();
	this->channels = new cuda::GpuMat[3];
	this->raw = new cuda::GpuMat();
	this->dst = new cuda::GpuMat();

	this->updaterThread = new pthread_t();
	this->frameLocker = new pthread_mutex_t();
	pthread_mutex_init(this->frameLocker, NULL);
}

App::~App()
{
	delete this->videoCapture;
	delete[] this->channels;
	delete this->raw;
	delete this->dst;
	delete this->dummyMat;
	delete this->targetDraw;
	delete this->updaterThread;
	delete this->criticalFrame;
	delete this->frameLocker;
}

int App::Run()
{
	double fps;
	long long start, elapsed;
	struct timespec begin, current;

	Mat *image = new Mat();

	UpdaterStruct info;
	info.frame = this->criticalFrame;
	info.frameLocker = this->frameLocker;
	info.streamAddress = &this->videoStreamAddress;
	info.vidCap = this->videoCapture;

	pthread_create(this->updaterThread, NULL, App::ReadFrameAsync, (void*)&info);

	bool notifiedWaiting = false;

	while(true)
	{
		clock_gettime(USED_CLOCK, &begin);
		start = begin.tv_sec * NANOS + begin.tv_nsec;

		pthread_mutex_lock(this->frameLocker);
		this->criticalFrame->copyTo(*image);
		pthread_mutex_unlock(this->frameLocker);

		if(image->empty())
		{
			if(!notifiedWaiting)
			{
				cerr << "Main thread: Waiting for image..." << endl;
				notifiedWaiting = true;
			}
			continue;
		}
		else
		{
			if(notifiedWaiting)
			{
				cerr << "Main thread: Got frame" << endl;
				notifiedWaiting = false;
			}
		}

		*image = this->PrepareFrame(*image);
		*image = this->FindTargets(*image);

		if(this->display)
		{
			imshow("Output Window", *image);
		}

		if(cv::waitKey(1) >= 0)
		{
			break;
		}

		 clock_gettime(USED_CLOCK, &current);
		 elapsed = current.tv_sec * NANOS + current.tv_nsec - start;
		 fps = 1 / (elapsed / NRATIO);
		 cerr << "\rMain thread: " <<  fps  << " FPS";

	}

	cerr << "" << endl;
	return 0;
}

void* App::ReadFrameAsync(void *arg)
{
	cerr << "Async reader: Starting..." << endl;

	 UpdaterStruct *info = (UpdaterStruct*) arg;
	 Mat *criticalFrame = info->frame;
	 VideoCapture *vidCap = info->vidCap;
	 pthread_mutex_t *locker = info->frameLocker;
	 string* streamAddress = info->streamAddress;

	if(!Utils::SafeVidCapOpen(*vidCap, *streamAddress))
	{
		cerr << "Async reader: Error opening the camera stream" << endl;
		return (void*)-1;
	}

	cerr << "Async reader: Camera opened successfully" << endl;

	Mat rawImage;

	while(true)
	{
		vidCap->read(rawImage);

		pthread_mutex_lock(locker);
		rawImage.copyTo(*criticalFrame);
		pthread_mutex_unlock(locker);
	}

	return (void*)0;
}

Mat& App::FindTargets(Mat& src)
{
	vector<vector<Point> > *contours = new vector<vector<Point> >();
	cv::findContours(src, *contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	int i = 0;
	for(vector<vector<Point> >::iterator it = contours->begin(); it != contours->end(); ++it, ++i)
	{
		if(this->debugDisplay)
		{
			cv::drawContours(src, *contours, i, Scalar(255,0,0));
			cv::imshow("FindTargets output", src);
		}
	}

	delete contours;
	return src;
}

Mat& App::PrepareFrame(Mat &src)
{
	this->raw->upload(src);

	cuda::split(*this->raw, this->channels);

	for(int i = 0; i < 3; i++)
	{
		cuda::threshold(this->channels[i], this->channels[i], MIN_THRESH, MAX_THRESH, cv::THRESH_BINARY);
	}

	cuda::bitwise_or(this->channels[0], this->channels[2], *this->dst);
	cuda::bitwise_not(*this->dst, *this->dst);

	cuda::bitwise_and(this->channels[1], *this->dst, *this->dst);

	int kernel_size = 3;
	Mat element = cv::getStructuringElement(MORPH_RECT, Size(2*kernel_size + 1, 2*kernel_size + 1), Point(kernel_size, kernel_size));

	Ptr<cuda::Filter> erodeFilter = cuda::createMorphologyFilter(MORPH_ERODE, this->dst->type(), element);
	erodeFilter->apply(*this->dst, *this->dst);

	Ptr<cuda::Filter> dilateFilter = cuda::createMorphologyFilter(MORPH_DILATE, this->dst->type(), element);
	dilateFilter->apply(*this->dst, *this->dst);

	this->dst->download(src);

	if(this->debugDisplay)
	{
		imshow("PrepareFrame result", src);
	}

	return src;
}
