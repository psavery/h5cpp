
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

TEST(ReadDataTest, getDims)
{
  H5Reader reader(test_file);
  vector<int> dims;

  EXPECT_TRUE(reader.getDims("/data/tomography/data", dims));
  EXPECT_EQ(dims.size(), 3);
  EXPECT_EQ(dims[0],  74);
  EXPECT_EQ(dims[1], 256);
  EXPECT_EQ(dims[2], 256);

  EXPECT_TRUE(reader.getDims("/data/tomography/dim1", dims));
  EXPECT_EQ(dims.size(), 1);
  EXPECT_EQ(dims[0], 74);

  EXPECT_TRUE(reader.getDims("/data/tomography/dim2", dims));
  EXPECT_EQ(dims.size(), 1);
  EXPECT_EQ(dims[0], 256);

  EXPECT_TRUE(reader.getDims("/data/tomography/dim3", dims));
  EXPECT_EQ(dims.size(), 1);
  EXPECT_EQ(dims[0], 256);
}

TEST(ReadDataTest, wrongType)
{
  H5Reader reader(test_file);
  vector<unsigned int> data;

  EXPECT_FALSE(reader.readData("/data/tomography/dim1", data));
}

TEST(ReadDataTest, getData)
{
  H5Reader reader(test_file);
  vector<float> angleData;

  EXPECT_TRUE(reader.readData("/data/tomography/dim1", angleData));

  // These should be -73 to 73, with a spacing of 2
  vector<float> comparison;
  for (int i = -73; i <= 73; i +=2)
    comparison.push_back(i);

  EXPECT_EQ(angleData.size(), comparison.size());
  for (size_t i = 0; i < angleData.size(); ++i)
    EXPECT_FLOAT_EQ(angleData[i], comparison[i]);

  vector<unsigned char> data;
  vector<int> dims;
  EXPECT_TRUE(reader.readData("/data/tomography/data", data, dims));
  EXPECT_EQ(dims.size(), 3);
  EXPECT_EQ(dims[0],  74);
  EXPECT_EQ(dims[1], 256);
  EXPECT_EQ(dims[2], 256);
}
