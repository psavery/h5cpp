/* This source file is part of the Tomviz project, https://tomviz.org/.
   It is released under the 3-Clause BSD License, see "LICENSE". */

#include <string>

#include <gtest/gtest.h>

#include <h5cpp/h5reader.h>

using std::string;

using tomviz::H5Reader;

static const string test_file = TESTDATADIR + string("/tomviz_tilt_ser.emd");

TEST(ReadAttributesTest, doesNotExist)
{
  H5Reader reader(test_file);
  int value;

  EXPECT_FALSE(reader.attribute("/does_not_exist/", "does_not_exist", value));
}

TEST(ReadAttributesTest, wrongType)
{
  H5Reader reader(test_file);
  int value;

  EXPECT_FALSE(reader.attribute("/data/tomography/dim1", "name", value));
}

TEST(ReadAttributesTest, readAttribute)
{
  H5Reader reader(test_file);
  string value;

  EXPECT_TRUE(reader.attribute("/data/tomography/dim1", "name", value));
  EXPECT_EQ(value, "angles");

  EXPECT_TRUE(reader.attribute("/data/tomography/dim1", "units", value));
  EXPECT_EQ(value, "[deg]");
}

TEST(ReadAttributesTest, getAttributeType)
{
  H5Reader reader(test_file);
  H5Reader::DataType type;

  EXPECT_TRUE(reader.attributeType("/data/tomography/dim1", "name", type));
  EXPECT_EQ(type, H5Reader::DataType::String);
}
