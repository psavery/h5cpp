
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include <h5cpp/h5reader.h>

using std::string;
using std::vector;

using tomviz::H5Reader;

static const string test_file = TESTDATADIR + string("/tomviz_tilt_ser.emd");

TEST(GetChildrenTest, doesNotExist)
{
  H5Reader reader(test_file);
  vector<string> result;

  EXPECT_FALSE(reader.children("/does_not_exist/", result));
}

TEST(GetChildrenTest, getChildren)
{
  H5Reader reader(test_file);
  vector<string> result;

  EXPECT_TRUE(reader.children("/", result));
  EXPECT_EQ(result.size(), 1);
  EXPECT_EQ(result[0], "data");

  EXPECT_TRUE(reader.children("/data", result));
  EXPECT_EQ(result.size(), 1);
  EXPECT_EQ(result[0], "tomography");

  EXPECT_TRUE(reader.children("/data/tomography", result));
  EXPECT_EQ(result.size(), 4);
  EXPECT_EQ(result[0], "data");
  EXPECT_EQ(result[1], "dim1");
  EXPECT_EQ(result[2], "dim2");
  EXPECT_EQ(result[3], "dim3");
}
