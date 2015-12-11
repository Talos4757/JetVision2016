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

    *(TargetType*)encoded = this->type;
    *(double*)(encoded + sizeof(TargetType)) = this->distance;
    *(double*)(encoded + sizeof(TargetType) + sizeof(double)) = this->horizontalAngle;
    *(double*)(encoded + sizeof(TargetType) + 2*sizeof(double)) = this->verticalAngle;

    return encoded;
}

Target Target::Deserialize(char* data)
{
    Target t;
    t.type = *(TargetType*)data;
    t.distance = *(double*)(data + sizeof(int));
    t.horizontalAngle = *(double*)(data + sizeof(int) + sizeof(double));
    t.verticalAngle = *(double*)(data + sizeof(int) + 2*sizeof(double));
    return t;
}

string Target::ToString()
{
    return "";
}
