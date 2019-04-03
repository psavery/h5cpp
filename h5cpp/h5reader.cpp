/* This source file is part of the Tomviz project, https://tomviz.org/.
   It is released under the 3-Clause BSD License, see "LICENSE". */

#include "h5reader.h"

#include <algorithm>
#include <iostream>
#include <map>
#include <numeric>

#include "h5capi.h"
#include "h5typemaps.h"
#include "hidcloser.h"

using std::cout;
using std::cerr;
using std::endl;

using std::map;
using std::string;
using std::vector;

namespace tomviz {

using DataType = H5Reader::DataType;

class H5Reader::H5ReaderImpl {
public:
  H5ReaderImpl()
  {
  }

  H5ReaderImpl(const string& file)
  {
    if (!openFile(file))
      cerr << "Warning: failed to open file " << file << "\n";
  }

  ~H5ReaderImpl()
  {
    clear();
  }

  bool openFile(const string& file)
  {
    m_fileId = H5Fopen(file.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
    return fileIsValid();
  }

  bool attributeExists(const string& group, const string& name)
  {
    if (!fileIsValid())
      return false;

    return H5Aexists_by_name(m_fileId, group.c_str(), name.c_str(),
                             H5P_DEFAULT) > 0;
  }

  bool attribute(const string& group, const string& name, void* value,
                 hid_t dataTypeId, hid_t memTypeId)
  {
    if (!attributeExists(group, name)) {
      cerr << "Attribute " << group << name << " not found!" << endl;
      return false;
    }

    hid_t attr = H5Aopen_by_name(m_fileId, group.c_str(), name.c_str(),
                                 H5P_DEFAULT, H5P_DEFAULT);
    hid_t type = H5Aget_type(attr);

    // For automatic closing upon leaving scope
    HIDCloser attrCloser(attr, H5Aclose);
    HIDCloser typeCloser(type, H5Tclose);

    if (H5Tequal(type, dataTypeId) == 0) {
      // The type of the attribute does not match the requested type.
      cerr << "Type determined does not match that requested." << endl;
      cerr << type << " -> " << dataTypeId << endl;
      return false;
    } else if (H5Tequal(type, dataTypeId) < 0) {
      cerr << "Something went really wrong....\n\n";
      return false;
    }

    return H5Aread(attr, memTypeId, value) >= 0;
  }

  // void* data needs to be of the appropiate type and size
  bool readData(const string& path, hid_t dataTypeId, hid_t memTypeId,
                void* data)
  {
    hid_t dataSetId = H5Dopen(m_fileId, path.c_str(), H5P_DEFAULT);
    if (dataSetId < 0) {
      cerr << "Failed to get dataSetId\n";
      return false;
    }

    // Automatically close upon leaving scope
    HIDCloser dataSetCloser(dataSetId, H5Dclose);

    hid_t dataSpaceId = H5Dget_space(dataSetId);
    if (dataSpaceId < 0) {
      cerr << "Failed to get dataSpaceId\n";
      return false;
    }

    HIDCloser dataSpaceCloser(dataSpaceId, H5Sclose);

    hid_t typeId = H5Dget_type(dataSetId);
    HIDCloser dataTypeCloser(typeId, H5Tclose);

    if (H5Tequal(typeId, dataTypeId) == 0) {
      // The type of the data does not match the requested type.
      cerr << "Type determined does not match that requested." << endl;
      cerr << typeId << " -> " << dataTypeId << endl;
      return false;
    } else if (H5Tequal(typeId, dataTypeId) < 0) {
      cerr << "Something went really wrong....\n\n";
      return false;
    }

    return H5Dread(dataSetId, memTypeId, H5S_ALL, dataSpaceId, H5P_DEFAULT,
                   data) >= 0;
  }

  bool getInfoByName(const string& path, H5O_info_t& info)
  {
    if (!fileIsValid())
      return false;

    return H5Oget_info_by_name(m_fileId, path.c_str(), &info,
                               H5P_DEFAULT) >= 0;
  }

  bool isDataSet(const string& path)
  {
    H5O_info_t info;
    if (!getInfoByName(path, info)) {
      cerr << "Failed to get H5O info by name\n";
      return false;
    }

    return info.type == H5O_TYPE_DATASET;
  }

  bool getH5ToDataType(hid_t h5type, DataType& type)
  {
    // Find the type
    auto it = std::find_if(H5ToDataType.cbegin(), H5ToDataType.cend(),
      [h5type](const std::pair<hid_t, DataType>& t)
      {
        return H5Tequal(t.first, h5type);
      });

    if (it == H5ToDataType.end()) {
      cerr << "H5ToDataType map does not contain H5 type: " << h5type
           << endl;
      return false;
    }

    type = it->second;
    return true;
  }

  bool fileIsValid() { return m_fileId >= 0; }

  void clear()
  {
    if (fileIsValid()) {
      H5Fclose(m_fileId);
      m_fileId = H5I_INVALID_HID;
    }
  }

  hid_t fileId() const { return m_fileId; }

  hid_t m_fileId = H5I_INVALID_HID;
};

H5Reader::H5Reader(const string& file)
: m_impl(new H5ReaderImpl(file))
{
}

H5Reader::~H5Reader() = default;

bool H5Reader::children(const string& path, vector<string>& result)
{
  if (!m_impl->fileIsValid())
    return false;

  constexpr int maxNameSize = 2048;
  char groupName[maxNameSize];

  hid_t groupId = H5Gopen(m_impl->fileId(), path.c_str(), H5P_DEFAULT);
  if (groupId < 0) {
    cerr << "Failed to open group: " << path << "\n";
    return false;
  }

  // For automatic closing upon leaving scope
  HIDCloser groupCloser(groupId, H5Gclose);

  hsize_t objCount = 0;
  H5Gget_num_objs(groupId, &objCount);

  result.clear();
  for (hsize_t i = 0; i < objCount; ++i) {
    H5Gget_objname_by_idx(groupId, i, groupName, maxNameSize);
    result.push_back(groupName);
  }

  return true;
}

template <typename T>
bool H5Reader::attribute(const string& group, const string& name, T& value)
{
  const hid_t dataTypeId = BasicTypeToH5<T>::dataTypeId();
  const hid_t memTypeId = BasicTypeToH5<T>::memTypeId();
  return m_impl->attribute(group, name, &value, dataTypeId, memTypeId);
}

// We have a specialization for std::string
template<>
bool H5Reader::attribute<string>(const string& group, const string& name,
                                 string& value)
{
  if (!m_impl->attributeExists(group, name)) {
    cerr << "Attribute " << group << name << " not found!" << endl;
    return false;
  }

  hid_t fileId = m_impl->fileId();

  hid_t attr = H5Aopen_by_name(fileId, group.c_str(), name.c_str(),
                               H5P_DEFAULT, H5P_DEFAULT);
  hid_t type = H5Aget_type(attr);

  // For automatic closing upon leaving scope
  HIDCloser attrCloser(attr, H5Aclose);
  HIDCloser typeCloser(type, H5Tclose);

  if (H5T_STRING != H5Tget_class(type)) {
    cerr << group << name << " is not a string" << endl;
    return false;
  }
  char* tmpString;
  int is_var_str = H5Tis_variable_str(type);
  if (is_var_str > 0) { // if it is a variable-length string
    if (H5Aread(attr, type, &tmpString) < 0) {
      cerr << "Failed to read attribute " << group << " " << name << endl;
      return false;
    }
    value = tmpString;
    free(tmpString);
  } else if (is_var_str == 0) { // If it is not a variable-length string
    // it must be fixed length since the "is a string" check earlier passed.
    size_t size = H5Tget_size(type);
    if (size == 0) {
      cerr << "Unknown error occurred" << endl;
      return false;
    }
    tmpString = new char[size + 1];
    if (H5Aread(attr, type, tmpString) < 0) {
      cerr << "Failed to read attribute " << group << " " << name << endl;
      delete [] tmpString;
      return false;
    }
    tmpString[size] = '\0'; // set null byte, hdf5 doesn't do this for you
    value = tmpString;
    delete [] tmpString;
  } else {
    cerr << "Unknown error occurred" << endl;
    return false;
  }

  return true;
}

bool H5Reader::attributeType(const string& group, const string& name,
                             DataType& type)
{
  if (!m_impl->attributeExists(group, name)) {
    cerr << "Attribute " << group << name << " not found!" << endl;
    return false;
  }

  hid_t fileId = m_impl->fileId();
  hid_t attr = H5Aopen_by_name(fileId, group.c_str(), name.c_str(),
                               H5P_DEFAULT, H5P_DEFAULT);
  hid_t h5type = H5Aget_type(attr);

  // For automatic closing upon leaving scope
  HIDCloser attrCloser(attr, H5Aclose);
  HIDCloser typeCloser(h5type, H5Tclose);

  // Special case for strings
  if (H5T_STRING == H5Tget_class(h5type)) {
    type = DataType::String;
    return true;
  }

  return m_impl->getH5ToDataType(h5type, type);
}

bool H5Reader::dataType(const string& path, DataType& type)
{
  if (!m_impl->isDataSet(path)) {
    cerr << path << " is not a data set.\n";
    return false;
  }

  hid_t dataSetId = H5Dopen(m_impl->fileId(), path.c_str(), H5P_DEFAULT);
  if (dataSetId < 0) {
    cerr << "Failed to get data set id\n";
    return false;
  }

  hid_t dataTypeId = H5Dget_type(dataSetId);

  // Automatically close
  HIDCloser dataSetCloser(dataSetId, H5Dclose);
  HIDCloser dataTypeCloser(dataTypeId, H5Tclose);

  return m_impl->getH5ToDataType(dataTypeId, type);
}

bool H5Reader::getDims(const string& path, vector<int>& dims)
{
  if (!m_impl->isDataSet(path)) {
    cerr << path << " is not a data set.\n";
    return false;
  }

  hid_t dataSetId = H5Dopen(m_impl->fileId(), path.c_str(), H5P_DEFAULT);
  if (dataSetId < 0) {
    cerr << "Failed to get dataSetId\n";
    return false;
  }

  // Automatically close upon leaving scope
  HIDCloser dataSetCloser(dataSetId, H5Dclose);

  hid_t dataSpaceId = H5Dget_space(dataSetId);
  if (dataSpaceId < 0) {
    cerr << "Failed to get dataSpaceId\n";
    return false;
  }

  HIDCloser dataSpaceCloser(dataSpaceId, H5Sclose);

  int dimCount = H5Sget_simple_extent_ndims(dataSpaceId);
  if (dimCount < 1) {
    cerr << "Error: number of dimensions is less than 1\n";
    return false;
  }

  hsize_t* h5dims = new hsize_t[dimCount];
  int dimCount2 = H5Sget_simple_extent_dims(dataSpaceId, h5dims, nullptr);

  if (dimCount != dimCount2) {
    cerr << "Error: dimCounts do not match\n";
    delete[] h5dims;
    return false;
  }

  dims.clear();
  dims.resize(dimCount);
  std::copy(h5dims, h5dims + dimCount, dims.begin());

  delete[] h5dims;

  return true;
}

bool H5Reader::dimensionCount(const string& path, int& nDims)
{
  vector<int> dims;
  if (!getDims(path, dims)) {
    cerr << "Failed to get the dimensions\n";
    return false;
  }

  nDims = dims.size();
  return true;
}

template <typename T>
bool H5Reader::readData(const string& path, vector<T>& result,
                        vector<int>& dims)
{
  const hid_t dataTypeId = BasicTypeToH5<T>::dataTypeId();
  const hid_t memTypeId = BasicTypeToH5<T>::memTypeId();

  if (!getDims(path, dims)) {
    cerr << "Failed to get the dimensions\n";
    return false;
  }

  // Multiply all the dimensions together
  auto size = std::accumulate(dims.cbegin(), dims.cend(), 1,
                              std::multiplies<int>());

  result.clear();
  result.resize(size);

  if (!m_impl->readData(path, dataTypeId, memTypeId, result.data())) {
    cerr << "Failed to read the data\n";
    return false;
  }

  return true;
}

template <typename T>
bool H5Reader::readData(const string& path, vector<T>& result)
{
  vector<int> dims;
  if (!readData(path, result, dims)) {
    cerr << "Failed to read the data\n";
    return false;
  }

  // Make sure there is one dimension
  if (dims.size() != 1) {
    cerr << "Warning: single-dimensional readData() called, but "
         << "multi-dimensional data was obtained.\n";
    cerr << "Number of dims is: " << dims.size() << "\n";
    return false;
  }

  return true;
}

template <typename T>
bool H5Reader::readData(const string& path, vector<vector<T>>& result)
{
  vector<T> data;
  vector<int> dims;
  if (!readData(path, data, dims)) {
    cerr << "Failed to read the data\n";
    return false;
  }

  // Make sure there are two dimensions
  if (dims.size() != 2) {
    cerr << "Warning: two-dimensional readData() called, but "
         << "two-dimensional data was not obtained.\n";
    cerr << "Number of dims is: " << dims.size() << "\n";
    return false;
  }

  // Just a sanity check
  if (static_cast<int>(data.size()) != dims[0] * dims[1]) {
    cerr << "Data size does not match dimensions!\n";
    return false;
  }

  result.clear();
  result.resize(dims[0]);
  for (auto& elem: result)
    elem.resize(dims[1]);

  for (int i = 0; i < dims[0]; ++i) {
    for (int j = 0; j < dims[1]; ++j) {
      result[i][j] = data[i * dims[1] + j];
    }
  }

  return true;
}

string H5Reader::dataTypeToString(const DataType& type)
{
  // Internal map. Keep it updated with the enum.
  static const map<DataType, const char*> DataTypeToString =
  {
    { DataType::Int8,   "Int8"   },
    { DataType::Int16,  "Int16"  },
    { DataType::Int32,  "Int32"  },
    { DataType::Int64,  "Int64"  },
    { DataType::UInt8,  "UInt8"  },
    { DataType::UInt16, "UInt16" },
    { DataType::UInt32, "UInt32" },
    { DataType::UInt64, "UInt64" },
    { DataType::Float,  "Float"  },
    { DataType::Double, "Double" },
    { DataType::String, "String" }
  };

  auto it = DataTypeToString.find(type);
  if (it == DataTypeToString.end())
    return "";

  return it->second;
}

// Instantiate our allowable templates here

// attribute()
template bool H5Reader::attribute(const string&, const string&, char&);
template bool H5Reader::attribute(const string&, const string&, short&);
template bool H5Reader::attribute(const string&, const string&, int&);
template bool H5Reader::attribute(const string&, const string&, long long&);
template bool H5Reader::attribute(const string&, const string&, unsigned char&);
template bool H5Reader::attribute(const string&, const string&,
                                  unsigned short&);
template bool H5Reader::attribute(const string&, const string&, unsigned int&);
template bool H5Reader::attribute(const string&, const string&,
                                  unsigned long long&);
template bool H5Reader::attribute(const string&, const string&, float&);
template bool H5Reader::attribute(const string&, const string&, double&);

template bool H5Reader::attribute(const string&, const string&, string&);

// readData(): single-dimensional
template bool H5Reader::readData(const string&, vector<char>&);
template bool H5Reader::readData(const string&, vector<short>&);
template bool H5Reader::readData(const string&, vector<int>&);
template bool H5Reader::readData(const string&, vector<long long>&);
template bool H5Reader::readData(const string&, vector<unsigned char>&);
template bool H5Reader::readData(const string&, vector<unsigned short>&);
template bool H5Reader::readData(const string&, vector<unsigned int>&);
template bool H5Reader::readData(const string&, vector<unsigned long long>&);
template bool H5Reader::readData(const string&, vector<float>&);
template bool H5Reader::readData(const string&, vector<double>&);

// readData(): two-dimensional
template bool H5Reader::readData(const string&, vector<vector<char>>&);
template bool H5Reader::readData(const string&, vector<vector<short>>&);
template bool H5Reader::readData(const string&, vector<vector<int>>&);
template bool H5Reader::readData(const string&, vector<vector<long long>>&);
template bool H5Reader::readData(const string&,
                                 vector<vector<unsigned char>>&);
template bool H5Reader::readData(const string&,
                                 vector<vector<unsigned short>>&);
template bool H5Reader::readData(const string&, vector<vector<unsigned int>>&);
template bool H5Reader::readData(const string&,
                                 vector<vector<unsigned long long>>&);
template bool H5Reader::readData(const string&, vector<vector<float>>&);
template bool H5Reader::readData(const string&, vector<vector<double>>&);

// readData(): multi-dimensional
template bool H5Reader::readData(const string&, vector<char>&, vector<int>&);
template bool H5Reader::readData(const string&, vector<short>&, vector<int>&);
template bool H5Reader::readData(const string&, vector<int>&, vector<int>&);
template bool H5Reader::readData(const string&, vector<long long>&,
                                 vector<int>&);
template bool H5Reader::readData(const string&, vector<unsigned char>&,
                                 vector<int>&);
template bool H5Reader::readData(const string&, vector<unsigned short>&,
                                 vector<int>&);
template bool H5Reader::readData(const string&, vector<unsigned int>&,
                                 vector<int>&);
template bool H5Reader::readData(const string&, vector<unsigned long long>&,
                                 vector<int>&);
template bool H5Reader::readData(const string&, vector<float>&, vector<int>&);
template bool H5Reader::readData(const string&, vector<double>&, vector<int>&);

// We need to create specializations for these
//template bool H5Reader::readData(const string&, vector<string>&);
//template bool H5Reader::readData(const string&, vector<vector<string>>&);
//template bool H5Reader::readData(const string&, vector<string>&,
//                                 vector<int>&);

} // namespace tomviz
