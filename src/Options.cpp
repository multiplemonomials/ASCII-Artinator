/*
 * Options.cpp
 *
 *  Created on: Dec 3, 2013
 *      Author: jamie
 */

#include "Options.h"
#include "Configuration.h"

Options::Options()
:_alliance(CURRENT_ALLIANCE),
 _armEnabled(ARM_ENABLED),
 _controllerPort(CONTROLLER_PORT)
{

}

Options & Options::instance()
{
	static Options instanceObject;

	return instanceObject;
}
