/*
 * Target.cpp
 *
 *  Created on: Dec 5, 2015
 *      Author: ubuntu
 */

#include "../types/Target.h"

Target::Target()
{
    this->type = NA;
    this->distance = 0;
    this->horizontalAngle = 0;
    this->verticalAngle = 0;

}

Target::~Target()
{

}

char* Target::Serialize()
{
    char* encoded = new char[TARGET_SIZE];

    *(int*)encoded = this->type;
    *(double*)(encoded+sizeof(int)) = this->distance;
    *(double*)(encoded+sizeof(int) + sizeof(double)) = this->horizontalAngle;
    *(double*)(encoded+sizeof(int) + 2*sizeof(double)) = this->verticalAngle;

    return encoded;
}
