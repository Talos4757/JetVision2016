#ifndef TARGET_H_
#define TARGET_H_

#include <iostream>
#include <string>

#include "../types/TargetType.h"

using namespace std;

class Target
{
public:
    Target();
    virtual ~Target();

    string ToString();
    char* Serialize();
    static Target Deserialize(char* data);

    TargetType type;
    double distance;
    double horizontalAngle;
    double verticalAngle;
};

#define TARGET_SIZE (sizeof(int) + 3*sizeof(double))

#endif /* TARGET_H_ */
