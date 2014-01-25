/*
 * vcard: a simple command line search tool for vcard contact file
 * Copyright (c) 2014 J. Knight
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * ----------------------------------------------------------------------- */


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
  /// this is done automatically by `parse` so if you
  /// want to change this, do it after calling `parse`
  string progname() { return pname; };
  void   progname( string name ) { pname = name; }

  /// parse c array 
  /// this expects the first argv[0] to be the program name
  /// @param argc the argument count
  /// @param argv the list of pointers to arg values
  bool parse( int argc, char **argv );
  
  /// parse an optlist
  bool parse( optlist args );
  
  /// determine if a option exists
  bool hasopt( string key );
  
  /// return option value or throw exception if 
  /// the option does not exist. Flags will return
  /// an empty string
  string optval( string key );
  
  /// get arg list
  /// anything without a '-' in front is treated as an
  /// arg and returned here.
  optlist getargs() { return args; }

  /// reset clears ann opts
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

