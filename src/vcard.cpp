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

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <list>
#include <algorithm>
#include <locale>
#include <cstdlib>

#include "version.h"
#include "optparser.h"

#define CRLF "\r\n"

/// case insensitive string found
bool findci(const std::string & haystack, const std::string & needle)
{
  auto it = std::search(
    haystack.begin(), haystack.end(),
    needle.begin(),   needle.end(),
    [](char ch1, char ch2) { return std::toupper(ch1) == std::toupper(ch2); }
  );
  if (it != haystack.end() ) { 
    return true;
  }
  return false;
}

/**
 * A vcard entry
 *
 * Basically just an attribute map. We just store the k/v pairs as we
 * find them.
 */
class VCardEntry
{
  public:
  VCardEntry()  {}
  ~VCardEntry() {}

  /// add attr to the map with value
  void addAttr( const std::string attr, const std::string value ) { _attribs[attr] = value; }

  /// look in all fields, print if found
  bool query( const std::string q, bool print = false );

  /// output the record
  void write( std::ostream &out );

  /// print the record to stdout a la mutt (<email><tab><fullname><tab><otherthing>)
  void print();

  /// return the actual key for a ci search for key
	std::vector<std::string> findKey( const std::string key );

  private:
  /// store stuffs
  std::map<std::string, std::string> _attribs;
};

void VCardEntry::write( std::ostream &out )
{
  out << "BEGIN:VCARD" << CRLF;
	out << "VERSION:" << _attribs["VERSION"] << CRLF;

  for( auto attr : _attribs ) {
		if( attr.first != "VERSION" )
			out << attr.first << ":" << attr.second << CRLF;
  }

  out << "END:VCARD" << CRLF;
}

void VCardEntry::print()
{
  std::stringstream ss;
	std::vector<std::string> tmp = findKey( "EMAIL" );
  if( tmp.size() > 0 ) {
		for( auto it : tmp ) {
			ss << _attribs[it];
			ss << "\t";
			if( _attribs.count("FN")) {
				ss << _attribs["FN"];
			}
			ss << "\t";
			tmp = findKey( "TEL" );
			if( tmp.size() > 0 ) {
				ss << _attribs[tmp[0]];
			}
			std::cout << ss.str() << std::endl;
			ss.str("");
		}
	}
}

std::vector<std::string> VCardEntry::findKey( const std::string key )
{
	std::vector<std::string> keylist;

  for( auto attr : _attribs ) {
    if( findci( attr.first, key ))
      keylist.push_back( attr.first );
  }
  return keylist;
}


bool VCardEntry::query( const std::string q, bool printit )
{
  for( auto attr : _attribs ) {
    if( findci( attr.second, q ) ) {
      if( printit ) {
        print();
      }
      return true;
    }
  }
  return false;
}

/**
 * A collection of vcard entries
 *
 */
class VCard
{
  public:
  VCard()  {}
  ~VCard();

  /// load entries from fname
  bool load( const std::string fname );

  /// save entries to fname
  bool save( const std::string fname );

  /// return number of entries
  int  count() { return _entries.size(); }

  /// search all fields for q
  int  query( const std::string q );

  private:
  std::list<VCardEntry *> _entries;
};

VCard::~VCard()
{
  for( auto it = _entries.begin(); it != _entries.end(); ) {
    delete *it;
    it = _entries.erase(it);
  }
}

bool VCard::load( const std::string fname )
{
  std::ifstream in( fname.c_str());
  if( !in.is_open() ) {
    return false;
  }

  VCardEntry *entry = nullptr;

	// TODO this doesn't handle folded lines properly
  std::string line;
  while( std::getline( in, line) ) {
    line.erase(line.find_last_not_of(" \n\r\t")+1);
    std::string key;
    std::string val;
    auto ndx = line.find_first_of( ':' );
    if( ndx != std::string::npos ) {
      key = line.substr( 0, ndx );
      val = line.substr( ndx+1 );
      if( key == "BEGIN" && val == "VCARD" ) {
        entry = new VCardEntry();
      }
      else if( key == "END" && val == "VCARD" ) {
        if( entry != nullptr )
          _entries.push_back( entry );
        entry = nullptr;
      }
      else {
        if( entry != nullptr )
          entry->addAttr( key, val );
      }
    }
  }

  in.close();
  return true;
}

bool VCard::save( const std::string fname )
{
  std::ofstream out( fname.c_str() );
  if( !out.is_open() ) {
    return false;
  }

  for( auto it = _entries.begin(); it != _entries.end(); it++ ) {
    (*it)->write( out );
  }

  out.close();
  return true;
}

int VCard::query( const std::string q )
{
  int count = 0;
  for( auto it = _entries.begin(); it != _entries.end(); it++ ) {
    if( (*it)->query( q, true ))
      count++;
  }
  return count;
}

void usage()
{
  std::cout << "vcard [OPTIONS] [ARGS]" << std::endl;
	std::cout << "any ARGS are run as a query" << std:: endl;
	std::cout << "\te.g. vcard Smith" << std::endl;
  std::cout << "OPTIONS are:" << std::endl;
  std::cout << "\t-v | --version" << std::endl;
  std::cout << "\t\tPrint version info and exit" << std::endl;
  std::cout << "\t-h | --help" << std::endl;
  std::cout << "\t\tPrint this screen" << std::endl;
  std::cout << "\t-d=DATAFILE | --datafile=DATAFILE" << std::endl;
  std::cout << "\t\tUse DATAFILE for data. Default is $HOME/.vcard/contacts.vcf" << std::endl;
  std::cout << std::endl;
	std::cout << "vcard:  Copyright (c) 2014 J. Knight" << std::endl;
	std::cout << "This program comes with ABSOLUTELY NO WARRANTY!" << std::endl;
	std::cout << "This is free software, licenced under the GPL v3 and you are" << std::endl;
	std::cout << "welcome to redistribute it under certain conditions." << std::endl;
	std::cout << "http://github.com/mtvee/vcard" << std::endl;

	std::cout << std::endl;
}

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 *                              M A I N
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */
int main( int argc, char **argv )
{
  // defaults
  std::string datafile = std::getenv("HOME");
  if( datafile.size() == 0 ) {
   std::cerr << "Unable to get $HOME" << std::endl;
   return 3;
  }

  datafile += "/.vcard/contacts.vcf";

  OptParser opt;
  if( !opt.parse( argc, argv )) {
    usage();
    return 1;
  }
  if( opt.hasopt("datafile")) {
    datafile = opt.optval( "datafile" );
  }
  if( opt.hasopt("version") || opt.hasopt("v") ) {
    std::cout << "vcard version " << APP_VERSION << " (" << APP_BUILD << ")" << std::endl;
    return 0;
  }
  if( opt.hasopt("help") || opt.hasopt("h") ) {
    usage();
    return 0;
  }

  VCard vc;
  
  if( !vc.load( datafile )) {
    std::cerr << "Unable to open data file: " << datafile << std::endl;
    return 2;
  }

  int count = 0;

  for( auto arg : opt.getargs()) {
    count += vc.query( arg );
  }

  if( count > 0 )
    return 0;
  else
    return 1;
}

