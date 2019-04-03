/* This source file is part of the Tomviz project, https://tomviz.org/.
   It is released under the 3-Clause BSD License, see "LICENSE". */

#include <string>
#include <vector>

#include <gtest/gtest.h>

#include <h5cpp/h5reader.h>

using std::string;
using std::vector;

using tomviz::H5Reader;

static const string test_file = TESTDATADIR + string("/tomviz_tilt_ser.emd");
static const string pmd_test_file = TESTDATADIR + string("/open_pmd_2d.h5");

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

  // One test for the string converter
  EXPECT_EQ(reader.dataTypeToString(type), "Float");
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

TEST(ReadDataTest, getDataTomviz)
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

TEST(ReadDataTest, getDataPmd)
{
  H5Reader reader(pmd_test_file);
  vector<vector<double>> fieldData;

  EXPECT_TRUE(reader.readData("/data/255/fields/rho", fieldData));

  EXPECT_EQ(fieldData.size(), 51);

  for (const auto& datum: fieldData)
    EXPECT_EQ(datum.size(), 201);

  // Pick a few samples to check
  EXPECT_DOUBLE_EQ(fieldData[0][0], 0.0);
  EXPECT_DOUBLE_EQ(fieldData[1][0], 480.786625502941430);
  EXPECT_DOUBLE_EQ(fieldData[2][3],  51.101970543191413);
}
