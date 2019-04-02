/* This source file is part of the Tomviz project, https://tomviz.org/.
   It is released under the 3-Clause BSD License, see "LICENSE". */

#include "h5reader.h"

#include <iostream>

#include "h5attributereader.h"
#include "h5capi.h"
#include "h5typemaps.h"
#include "h5typereader.h"

using std::cout;
using std::cerr;
using std::endl;

using std::string;

namespace tomviz {

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

template <typename T>
bool H5Reader::attribute(const string& group, const string& name, T& value)
{
  if (!m_impl->fileIsValid())
    return false;

  hid_t fileId = m_impl->fileId();
  if (H5Aexists_by_name(fileId, group.c_str(), name.c_str(), H5P_DEFAULT) <=
      0) {
    // The specified attribute does not exist.
    cerr << group << name << " not found!" << endl;
    return false;
  }

  H5AttributeReader attrReader(fileId, group.c_str(), name.c_str());
  H5TypeReader typeReader(attrReader.type());

  hid_t attr = attrReader.attr();
  hid_t type = typeReader.type();
  const hid_t typeId = BasicTypeToH5<T>::dataTypeId();
  if (H5Tequal(type, typeId) == 0) {
    // The type of the attribute does not match the requested type.
    cerr << "Type determined does not match that requested." << endl;
    cerr << type << " -> " << typeId << endl;
    return false;
  } else if (H5Tequal(type, typeId) < 0) {
    cerr << "Something went really wrong....\n\n";
    return false;
  }
  hid_t status = H5Aread(attr, BasicTypeToH5<T>::memTypeId(), &value);
  return status >= 0;
}

// We have a specialization for std::string
template<>
bool H5Reader::attribute<std::string>(const std::string& group,
                                      const std::string& name,
                                      std::string& value)
{
  if (!m_impl->fileIsValid())
    return false;

  hid_t fileId = m_impl->fileId();
  if (H5Aexists_by_name(fileId, group.c_str(), name.c_str(), H5P_DEFAULT) <=
      0) {
    // The specified attribute does not exist.
    cout << group << name << " not found!" << endl;
    return false;
  }

  H5AttributeReader attrReader(fileId, group.c_str(), name.c_str());
  H5TypeReader typeReader(attrReader.type());

  hid_t attr = attrReader.attr();
  hid_t type = typeReader.type();
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
