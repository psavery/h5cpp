
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

TEST(ReadDataTest, numDims)
{
  H5Reader reader(test_file);
  int nDims;

  EXPECT_TRUE(reader.numDims("/data/tomography/data", nDims));
  EXPECT_EQ(nDims, 3);

  EXPECT_TRUE(reader.numDims("/data/tomography/dim1", nDims));
  EXPECT_EQ(nDims, 1);

  EXPECT_TRUE(reader.numDims("/data/tomography/dim2", nDims));
  EXPECT_EQ(nDims, 1);

  EXPECT_TRUE(reader.numDims("/data/tomography/dim3", nDims));
  EXPECT_EQ(nDims, 1);
}

TEST(ReadDataTest, getData)
{
  H5Reader reader(test_file);
  vector<float> data;

  EXPECT_TRUE(reader.readData("/data/tomography/dim1", data));

  // These should be -73 to 73, with a spacing of 2
  vector<float> comparison;
  for (int i = -73; i <= 73; i +=2)
    comparison.push_back(i);

  EXPECT_EQ(data.size(), comparison.size());
  for (size_t i = 0; i < data.size(); ++i)
    EXPECT_FLOAT_EQ(data[i], comparison[i]);
}
