/* This source file is part of the Tomviz project, https://tomviz.org/.
   It is released under the 3-Clause BSD License, see "LICENSE". */

#include <string>
#include <vector>

#include <gtest/gtest.h>

#include <h5cpp/h5readwrite.h>

using std::string;
using std::vector;

using tomviz::H5ReadWrite;

static const string test_file = TESTDATADIR + string("/tomviz_tilt_ser.emd");

TEST(GetChildrenTest, doesNotExist)
{
  H5ReadWrite reader(test_file);
  bool ok;
  vector<string> result = reader.children("/does_not_exist/", &ok);

  EXPECT_FALSE(ok);
  EXPECT_TRUE(result.empty());
}

TEST(GetChildrenTest, getChildren)
{
  H5ReadWrite reader(test_file);
  bool ok;

  vector<string> result = reader.children("/", &ok);

  EXPECT_TRUE(ok);
  EXPECT_EQ(result.size(), 1);
  EXPECT_EQ(result[0], "data");

  result = reader.children("/data", &ok);

  EXPECT_TRUE(ok);
  EXPECT_EQ(result.size(), 1);
  EXPECT_EQ(result[0], "tomography");

  result = reader.children("/data/tomography", &ok);

  EXPECT_TRUE(ok);
  EXPECT_EQ(result.size(), 4);
  EXPECT_EQ(result[0], "data");
  EXPECT_EQ(result[1], "dim1");
  EXPECT_EQ(result[2], "dim2");
  EXPECT_EQ(result[3], "dim3");
}
