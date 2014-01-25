
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <list>
#include <algorithm>
#include <locale>
#include <cstdlib>

#include "config.h"
#include "optparser.h"

/// case insensitive find string
bool findci(const std::string & strHaystack, const std::string & strNeedle)
{
  auto it = std::search(
    strHaystack.begin(), strHaystack.end(),
    strNeedle.begin(),   strNeedle.end(),
    [](char ch1, char ch2) { return std::toupper(ch1) == std::toupper(ch2); }
  );
  if (it != strHaystack.end() ) { 
    return true;
  }
  return false;
}

/**
 * A vcard entry
 *
 * Basically just an attribute map
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
  /// print the record to stdout
  void print();
  /// return the actual key for a ci search for key
  std::string findKey( const std::string key );

  private:
  /// store stuffs
  std::map<std::string, std::string> _attribs;
};

void VCardEntry::write( std::ostream &out )
{
  out << "BEGIN:VCARD" << std::endl;

  for( auto attr : _attribs ) {
    out << attr.first << ":" << attr.second << std::endl;
  }

  out << "END:VCARD" << std::endl;
}

void VCardEntry::print()
{
  std::stringstream ss;
  std::string tmp = findKey( "EMAIL" );
  if( tmp.size() > 0 ) {
    ss << _attribs[tmp];
  }
  ss << "\t";
  if( _attribs.count("FN")) {
    ss << _attribs["FN"];
  }
  ss << "\t";
  std::cout << ss.str() << std::endl;
}

std::string VCardEntry::findKey( const std::string key )
{
  for( auto attr : _attribs ) {
    if( findci( attr.first, key ))
      return attr.first;
  }
  return "";
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

  bool load( const std::string fname );
  bool save( const std::string fname );
  int  count() { return _entries.size(); }
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

  std::string line;
  while( std::getline( in, line) ) {
    line.erase(line.find_last_not_of(" \n\r\t")+1);
    //std::cout << "[" << line << "]" << std::endl;
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
  std::cout << "where OPTIONS are:" << std::endl;
  std::cout << "/t-v | --version" << std::endl;
  std::cout << "/t/tPrint version info and exit" << std::endl;
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

  for( int i = 1; i < argc; i++ ) {
    count += vc.query( argv[i] );
  }

  if( count > 0 )
    return 0;
  else
    return 1;
}
