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

using namespace std;

class Target
{
public:
	Target();
	virtual ~Target();
	string Serialize();
};

#endif /* TARGET_H_ */
