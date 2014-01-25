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


#include "optparser.h"

void OptParser::reset()
{
  options.clear();
  args.clear();
  pname.clear();
}

bool OptParser::hasopt( string key )
{
  return (options.count(key) != 0);
}

string OptParser::optval( string key )
{
  auto it = options.find( key );
  if( it != options.end() ) {
    return it->second;
  } else {
    throw OptParserException("option not found!");
  }
}

void OptParser::dump()
{
  cout << "options:" << endl;
  cout << "-------" << endl;
  for( const auto &mit : options ) {
    cout << "\t" << mit.first << " [" << mit.second << "]" << endl;
  }
  cout << endl;

  cout << "args:" << endl;
  cout << "----" << endl;
  for( const auto it : args ) {
    cout << "\t" << it << endl;
  }
}

bool OptParser::parse( int argc, char **argv )
{
  optlist args;

  // grab the program name
  pname = argv[0];

  for( int i = 1; i < argc; i++ ) {
    args.push_back( argv[i] );
  }

  return parse( args );
}

bool OptParser::parse( optlist margs )
{
  string thisopt;

  for( const auto it : margs ) {
    thisopt = it;
    // if it's an option
    if( thisopt[0] == '-' ) {
      // eat the dashes
      while(thisopt[0] == '-' && thisopt.size()) {
        thisopt = thisopt.substr(1);
      }
      // see if this opt has an argument
      size_t ndx = thisopt.find_first_of('=');
      if( ndx != string::npos ) {
        options[thisopt.substr(0,ndx)] = thisopt.substr(ndx+1);
      } else {
        options[thisopt] = "";
      }
    } 
    // else it a plain argument
    else {
      args.push_back( thisopt );
    }
  }
  return true;
}



