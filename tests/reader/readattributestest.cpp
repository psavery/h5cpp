/* This source file is part of the Tomviz project, https://tomviz.org/.
   It is released under the 3-Clause BSD License, see "LICENSE". */

#include <string>

#include <gtest/gtest.h>

#include <h5cpp/h5readwrite.h>

using std::string;

using tomviz::H5ReadWrite;

static const string test_file = TESTDATADIR + string("/tomviz_tilt_ser.emd");

TEST(ReadAttributesTest, doesNotExist)
{
  H5ReadWrite reader(test_file);
  bool ok;

  reader.attribute<int>("/does_not_exist/", "does_not_exist", &ok);

  EXPECT_FALSE(ok);
}

TEST(ReadAttributesTest, wrongType)
{
  H5ReadWrite reader(test_file);
  bool ok;

  reader.attribute<int>("/data/tomography/dim1", "name", &ok);
  EXPECT_FALSE(ok);
}

TEST(ReadAttributesTest, hasAttribute)
{
  H5ReadWrite reader(test_file);

  EXPECT_FALSE(reader.hasAttribute("/data"));
  EXPECT_TRUE(reader.hasAttribute("/data/tomography/dim1"));

  EXPECT_FALSE(reader.hasAttribute("/data/tomography/dim1", "DNE"));
  EXPECT_TRUE(reader.hasAttribute("/data/tomography/dim1", "name"));
}

TEST(ReadAttributesTest, readAttribute)
{
  H5ReadWrite reader(test_file);
  bool ok;

  string value = reader.attribute<string>("/data/tomography/dim1", "name", &ok);

  EXPECT_TRUE(ok);
  EXPECT_EQ(value, "angles");

  value = reader.attribute<string>("/data/tomography/dim1", "units", &ok);

  EXPECT_TRUE(ok);
  EXPECT_EQ(value, "[deg]");
}

TEST(ReadAttributesTest, getAttributeType)
{
  H5ReadWrite reader(test_file);

  H5ReadWrite::DataType type = reader.attributeType("/data/tomography/dim1",
                                                 "name");
  EXPECT_EQ(type, H5ReadWrite::DataType::String);
}
