
#ifndef __OPTPARSER_H__
#define __OPTPARSER_H__

#include <map>
#include <vector>
#include <iostream>
#include <stdexcept>

using namespace std;

typedef vector<string> optlist;
typedef map<string, string> optmap;

/**
 * This is thrown if you try to get a value for an option
 * that does not exist.
 */
class OptParserException : public std::runtime_error 
{
 public:
   OptParserException( string msg ) : std::runtime_error( msg ) { }
};

/**
 * A basic non-posix option parser for command line stuff. 
 * 
 * This does not handle option arguments like posix or getopt. Option
 * arguments must be contiguous and separated with a '=', no spaces!  
 * e.g. --filename=myfile.dat  - or -  -filename=myfile.dat
 *
 * It also doesn't care about short or long options or how many
 * '-' you put in front. e.g. -----filename=myfile.dat
 *      
 * Flags will come back as an empty string if you try and get a value
 *
 * This simplifies the parsing greatly and you don't have to tell the 
 * parser whether an option requires an arg or not. Logic and error 
 * checking are up to you.
 */
class OptParser
{
  public:
  OptParser() {};
  ~OptParser() {};

  /// get/set the program name (argv[0])
  string progname() { return pname; };
  void   progname( string name ) { pname = name; }

  /// parse c array 
  bool parse( int argc, char **argv );
  
  /// parse an optlist
  bool parse( optlist args );
  
  /// determine if a option exists
  bool hasopt( string key );
  
  /// return option value or throw exception if 
  /// the option does not exist
  string optval( string key );
  
  /// get arg list
  optlist getargs() { return args; }

  /// reset
  void reset();

  // debug output to cout
  void dump();

  /// rest of the args
  optlist args;     
  
  private:
  string  pname;    // program name
  optmap  options;  // options map
};


#endif

