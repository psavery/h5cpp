
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include <h5cpp/h5reader.h>

using std::string;
using std::vector;

using tomviz::H5Reader;

static const string test_file = TESTDATADIR + string("/sample.h5");

TEST(ReadDataTest, getDataType)
{
  H5Reader reader(test_file);
  H5Reader::DataType type;

  EXPECT_TRUE(reader.dataType("/data/tomography/data", type));
  EXPECT_EQ(type, H5Reader::DataType::UInt8);

  EXPECT_TRUE(reader.dataType("/data/tomography/dim1", type));
  EXPECT_EQ(type, H5Reader::DataType::Float);

  EXPECT_TRUE(reader.dataType("/data/tomography/dim2", type));
  EXPECT_EQ(type, H5Reader::DataType::Float);

  EXPECT_TRUE(reader.dataType("/data/tomography/dim3", type));
  EXPECT_EQ(type, H5Reader::DataType::Float);
}
