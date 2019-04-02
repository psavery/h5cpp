/* This source file is part of the Tomviz project, https://tomviz.org/.
   It is released under the 3-Clause BSD License, see "LICENSE". */

#include "h5reader.h"

#include <iostream>
#include <map>

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
    hid_t status = H5Aread(attr, memTypeId, value);
    return status >= 0;
  }

  bool getInfoByName(const string& path, H5O_info_t& info)
  {
    if (!fileIsValid())
      return false;

    // Verify that the path exists in the HDF5 file.
    if (H5Oget_info_by_name(m_fileId, path.c_str(), &info, H5P_DEFAULT < 0))
      return false;

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
  result.clear();

  if (!m_impl->fileIsValid())
    return false;

  constexpr int maxNameSize = 2048;
  char groupName[maxNameSize];
  HIDCloser group(H5Gopen(m_impl->fileId(), path.c_str(), H5P_DEFAULT), H5Gclose);
  hid_t groupId = group.value();

  if (groupId < 0) {
    cerr << "Failed to open group: " << path << "\n";
    return false;
  }

  hsize_t objCount = 0;
  H5Gget_num_objs(groupId, &objCount);
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
    cout << group << name << " is not a string" << endl;
    return false;
  }
  // TODO: make sure tmpString is being allocated and freed properly
  char* tmpString;
  int is_var_str = H5Tis_variable_str(type);
  if (is_var_str > 0) { // if it is a variable-length string
    if (H5Aread(attr, type, &tmpString) < 0) {
      cout << "Failed to read attribute " << group << " " << name << endl;
      return false;
    }
  } else if (is_var_str == 0) { // If it is not a variable-length string
    // it must be fixed length since the "is a string" check earlier passed.
    size_t size = H5Tget_size(type);
    if (size == 0) {
      cout << "Unknown error occurred" << endl;
      return false;
    }
    tmpString = new char[size + 1];
    if (H5Aread(attr, type, tmpString) < 0) {
      cout << "Failed to read attribute " << group << " " << name << endl;
      delete tmpString;
      return false;
    }
    tmpString[size] = '\0'; // set null byte, hdf5 doesn't do this for you
  } else {
    cout << "Unknown error occurred" << endl;
    return false;
  }
  value = tmpString;
  free(tmpString);
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

  // Ensure that the map contains the key
  auto it = H5ToDataType.find(h5type);
  if (it == H5ToDataType.end()) {
    cerr << "H5ToDataType map does not contain key H5 type: " << h5type
         << endl;
    return false;
  }

  type = it->second;
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

} // namespace tomviz
