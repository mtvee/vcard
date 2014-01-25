#include <iostream>
#include "qunit.h"

#include "optparser.h"

class testopt
{
  private:
  QUnit::UnitTest qunit;

  void testParser() 
  {
    optlist opts;
    opts.push_back("--filename=foo.txt");

    OptParser op;
    op.parse( opts );

    QUNIT_IS_TRUE( op.hasopt("filename") );
    QUNIT_IS_TRUE( op.optval("filename") == "foo.txt" );
  }


public:
  testopt (std::ostream &out, int v = QUnit::quiet )
    : qunit(out,v) {}
  
  int run() {
    testParser();
    return qunit.errors();
  }

};

int main() {
  return testopt(std::cerr).run();
}
