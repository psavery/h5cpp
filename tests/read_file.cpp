
#include <iostream>
#include <string>

#include <h5cpp/h5reader.h>

using std::cout;
using std::cerr;

using std::string;

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

bool test_read_attribute(H5Reader& reader)
{
  string value;
  if (!reader.attribute("/data/tomography/dim1", "name", value)) {
    std::cerr << "Failed to read name of dim1\n";
    return false;
  }

  if (value != "angles") {
    std::cerr << "name of dim1 should be 'angles', but it is instead: "
              << value << "\n";
    return false;
  }

  if (!reader.attribute("/data/tomography/dim1", "units", value)) {
    std::cerr << "Failed to read units of dim1\n";
    return false;
  }

  if (value != "[deg]") {
    std::cerr << "units of dim1 should be '[deg]', but it is instead: "
              << value << "\n";
    return false;
  }

  return true;
}

bool test_attribute_reader(H5Reader& reader)
{
  if (!test_attribute_does_not_exist(reader)) {
    cerr << "Failed 'attribute does not exist' test!\n";
    return false;
  }

  if (!test_attribute_wrong_type(reader)) {
    cerr << "Failed 'attribute wrong type' test!\n";
    return false;
  }

  if (!test_read_attribute(reader)) {
    cerr << "Failed 'read attribute' test!\n";
    return false;
  }

  return true;
}

int main()
{
  std::string test_file = TESTDATADIR + std::string("/sample.h5");
  H5Reader reader(test_file);

  if (!test_attribute_reader(reader)) {
    cerr << "Attribute reader tests failed!\n";
    return 1;
  }

  cout << "Tests all passed!\n";
  return 0;
}
