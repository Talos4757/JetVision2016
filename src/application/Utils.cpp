/*
 * Utils.cpp
 *
 *  Created on: Dec 5, 2015
 *      Author: ubuntu
 */

#include "Utils.h"

Point Utils::GetCenterOfMat(Mat& mat)
{
	Size s = mat.size();
	return Point(s.width / 2, s.height / 2);
}

bool Utils::SafeVidCapOpen(VideoCapture& videoCapture, const string& target)
{
	bool result;

	try
	{
		Mat *check = new Mat();
		result = videoCapture.open(target);
		videoCapture.read(*check);
		delete check;
	}
	catch (...)
	{
		result = false;
	}

	return result;
}


