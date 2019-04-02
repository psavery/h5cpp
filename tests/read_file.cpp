
#include <iostream>
#include <string>

#include <h5cpp/h5reader.h>

using std::cout;
using std::cerr;

using tomviz::H5Reader;

bool test_attribute_does_not_exist(H5Reader& reader)
{
  int value;
  return !reader.attribute("/does_not_exist/", "does_not_exist", value);
}

bool test_attribute_wrong_type(H5Reader& reader)
{
  int value;
  return !reader.attribute("/data/tomography/dim1", "name", value);
}

int main()
{
  std::string test_file = TESTDATADIR + std::string("/sample.h5");
  H5Reader reader(test_file);

  if (!test_attribute_does_not_exist(reader)) {
    cout << "Failed attribute does not exist test!\n";
    return 1;
  }

  if (!test_attribute_wrong_type(reader)) {
    cout << "Failed attribute wrong type test!\n";
    return 1;
  }

  cout << "Tests all passed!\n";
  return 0;
}
