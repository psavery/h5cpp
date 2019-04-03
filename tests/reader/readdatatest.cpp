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

TEST(ReadDataTest, isDataSet)
{
  H5Reader reader(test_file);

  EXPECT_FALSE(reader.isDataSet("/data"));
  EXPECT_TRUE(reader.isDataSet("/data/tomography/data"));
}

TEST(ReadDataTest, getDataType)
{
  H5Reader reader(test_file);

  H5Reader::DataType type = reader.dataType("/data/tomography/data");
  EXPECT_EQ(type, H5Reader::DataType::UInt8);

  type = reader.dataType("/data/tomography/dim1");
  EXPECT_EQ(type, H5Reader::DataType::Float);

  type = reader.dataType("/data/tomography/dim2");
  EXPECT_EQ(type, H5Reader::DataType::Float);

  type = reader.dataType("/data/tomography/dim3");
  EXPECT_EQ(type, H5Reader::DataType::Float);

  // One test for the string converter
  EXPECT_EQ(reader.dataTypeToString(type), "Float");
}

TEST(ReadDataTest, dimensionCount)
{
  H5Reader reader(test_file);
  int nDims = reader.dimensionCount("/data/tomography/data");
  EXPECT_EQ(nDims, 3);

  nDims = reader.dimensionCount("/data/tomography/dim1");
  EXPECT_EQ(nDims, 1);

  nDims = reader.dimensionCount("/data/tomography/dim2");
  EXPECT_EQ(nDims, 1);

  nDims = reader.dimensionCount("/data/tomography/dim3");
  EXPECT_EQ(nDims, 1);
}

TEST(ReadDataTest, getDimensions)
{
  H5Reader reader(test_file);
  vector<int> dims = reader.getDimensions("/data/tomography/data");
  EXPECT_EQ(dims.size(), 3);
  EXPECT_EQ(dims[0],  74);
  EXPECT_EQ(dims[1], 256);
  EXPECT_EQ(dims[2], 256);

  dims = reader.getDimensions("/data/tomography/dim1");
  EXPECT_EQ(dims.size(), 1);
  EXPECT_EQ(dims[0], 74);

  dims = reader.getDimensions("/data/tomography/dim2");
  EXPECT_EQ(dims.size(), 1);
  EXPECT_EQ(dims[0], 256);

  dims = reader.getDimensions("/data/tomography/dim3");
  EXPECT_EQ(dims.size(), 1);
  EXPECT_EQ(dims[0], 256);
}

TEST(ReadDataTest, wrongType)
{
  H5Reader reader(test_file);
  vector<unsigned int> data =
    reader.readData<unsigned int>("/data/tomography/dim1");

  EXPECT_TRUE(data.empty());
}

TEST(ReadDataTest, getDataTomviz)
{
  H5Reader reader(test_file);
  vector<float> angleData = reader.readData<float>("/data/tomography/dim1");

  EXPECT_FALSE(angleData.empty());

  // These should be -73 to 73, with a spacing of 2
  vector<float> comparison;
  for (int i = -73; i <= 73; i +=2)
    comparison.push_back(i);

  EXPECT_EQ(angleData.size(), comparison.size());
  for (size_t i = 0; i < angleData.size(); ++i)
    EXPECT_FLOAT_EQ(angleData[i], comparison[i]);

  vector<int> dims;
  vector<unsigned char> data =
    reader.readData<unsigned char>("/data/tomography/data", dims);
  EXPECT_EQ(dims.size(), 3);
  EXPECT_EQ(dims[0],  74);
  EXPECT_EQ(dims[1], 256);
  EXPECT_EQ(dims[2], 256);

  // Let's reshape it into a 3D vector
  vector<vector<vector<double>>> data3D;
  data3D.resize(dims[0]);
  for (auto& datum: data3D) {
    datum.resize(dims[1]);
    for (auto & innerDatum: datum)
      innerDatum.resize(dims[2]);
  }

  for (int i = 0; i < dims[0]; ++i) {
    for (int j = 0; j < dims[1]; ++j) {
      for (int k = 0; k < dims[2]; ++k) {
        data3D[i][j][k] = data[(i * dims[1] + j) * dims[2] + k];
      }
    }
  }
  // Done reshaping...

  // Pick a few samples to check
  EXPECT_EQ(data3D[0][0][0], 5);
  EXPECT_EQ(data3D[0][5][23], 8);
  EXPECT_EQ(data3D[4][5][22], 1);
}

TEST(ReadDataTest, getDataPmd)
{
  H5Reader reader(pmd_test_file);

  vector<int> dims;
  vector<double> fieldData = reader.readData<double>("/data/255/fields/rho",
                                                     dims);

  EXPECT_EQ(dims.size(), 2);

  EXPECT_EQ(dims[0], 51);
  EXPECT_EQ(dims[1], 201);

  // Let's reshape it into a vector of vectors
  vector<vector<double>> data2D;
  data2D.resize(dims[0]);
  for (auto& datum: data2D)
    datum.resize(dims[1]);

  for (int i = 0; i < dims[0]; ++i) {
    for (int j = 0; j < dims[1]; ++j) {
      data2D[i][j] = fieldData[i * dims[1] + j];
    }
  }
  // Done reshaping...

  // Pick a few samples to check
  EXPECT_DOUBLE_EQ(data2D[0][0], 0.0);
  EXPECT_DOUBLE_EQ(data2D[1][0], 480.786625502941430);
  EXPECT_DOUBLE_EQ(data2D[2][3],  51.101970543191413);
}

TEST(ReadDataTest, getDataViaPointer)
{
  H5Reader reader(pmd_test_file);

  vector<int> dims = reader.getDimensions("/data/255/fields/rho");

  EXPECT_EQ(dims.size(), 2);

  EXPECT_EQ(dims[0], 51);
  EXPECT_EQ(dims[1], 201);

  size_t size = dims[0] * dims[1];

  vector<double> data;
  data.resize(size);

  EXPECT_TRUE(reader.readData<double>("/data/255/fields/rho", data.data()));

  // Let's reshape it into a vector of vectors
  vector<vector<double>> data2D;
  data2D.resize(dims[0]);
  for (auto& datum: data2D)
    datum.resize(dims[1]);

  for (int i = 0; i < dims[0]; ++i) {
    for (int j = 0; j < dims[1]; ++j) {
      data2D[i][j] = data[i * dims[1] + j];
    }
  }
  // Done reshaping...

  // Pick a few samples to check
  EXPECT_DOUBLE_EQ(data2D[0][0], 0.0);
  EXPECT_DOUBLE_EQ(data2D[1][0], 480.786625502941430);
  EXPECT_DOUBLE_EQ(data2D[2][3],  51.101970543191413);
}
