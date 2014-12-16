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

	//options are initialized by Main
	Options();

    Options(Options const&);              // Don't Implement
    void operator=(Options const&); // Don't implement

public:

   //pixels in either axis per char in the output
   unsigned int blockSize;

   //enable debug logging
   bool debugOut;

   //enable info logging
   bool infoOut;

   bool scaleCharValues;

   static Options & instance();


};

#endif /* OPTIONS_H_ */
