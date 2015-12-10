/*
 * Target.h
 *
 *  Created on: Dec 5, 2015
 *      Author: ubuntu
 */

#ifndef TARGET_H_
#define TARGET_H_

#include <stdio.h>
#include <string>

#include "../types/TargetType.h"

using namespace std;

class Target
{
public:
    Target();
    virtual ~Target();
    char* Serialize();

    TargetType type;
    double distance;
    double horizontalAngle;
    double verticalAngle;
};

#define TARGET_SIZE (sizeof(int) + 3*sizeof(double))

#endif /* TARGET_H_ */
