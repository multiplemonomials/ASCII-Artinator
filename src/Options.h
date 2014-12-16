/*
 * This singleton class holds options for the program.  They are initialized from
 * the config file, and can be changed at runtime.
 *
 *  Created on: Dec 3, 2013
 *      Author: jamie
 */

#ifndef OPTIONS_H_
#define OPTIONS_H_

#include <string>
#include <map>
#include <memory>

//forward-declare this so Connection can include it
class Connection;

class Options
{
private:
	Options();

    Options(Options const&);              // Don't Implement
    void operator=(Options const&); // Don't implement

public:

    enum class Alliance
    {
    	RED,
    	BLUE
    };

    Alliance _alliance;

    bool _armEnabled;

    //port the controller connected to the drivers' console is on
    short _controllerPort;

	static Options & instance();


};

#endif /* OPTIONS_H_ */
