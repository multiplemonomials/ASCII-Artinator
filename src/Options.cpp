/*
 * Options.cpp
 *
 *  Created on: Dec 3, 2013
 *      Author: jamie
 */

#include "Options.h"


Options::Options()
:version(1)
{

}

Options & Options::instance()
{
	static Options instanceObject;

	return instanceObject;
}
