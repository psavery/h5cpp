/* This source file is part of the Tomviz project, https://tomviz.org/.
   It is released under the 3-Clause BSD License, see "LICENSE". */

#include "h5reader.h"

#include <iostream>

#include "h5capi.h"
#include "h5typemaps.h"

using std::cout;
using std::cerr;
using std::string;

namespace tomviz {

class H5ReaderImpl {
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
    if (fileIsValid()) {
      H5Fclose(m_fileId);
      m_fileId = H5I_INVALID_HID;
    }
  }

  bool openFile(const string& file)
  {
    m_fileId = H5Fopen(file.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
    return fileIsValid();
  }

  bool fileIsValid()
  {
    if (m_fileId == H5I_INVALID_HID || m_fileId < 0)
      return false;

    return true;
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

  hid_t fileId() const { return m_fileId; }

  hid_t m_fileId = H5I_INVALID_HID;
};

H5Reader::H5Reader(const string& file)
: m_impl(new H5ReaderImpl(file))
{
}

H5Reader::~H5Reader() = default;

template <typename T>
bool H5Reader::attribute<T>(const string& group, const string& name, T& value)
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

  hid_t attr = H5Aopen_by_name(fileId, group.c_str(), name.c_str(),
                               H5P_DEFAULT, H5P_DEFAULT);
  hid_t type = H5Aget_type(attr);
  const typeId = BasicTypeToH5<T>::dataTypeId;
  if (H5Tequal(type, typeId) == 0) {
    // The type of the attribute does not match the requested type.
    cerr << "Type determined does not match that requested." << endl;
    cerr << type << " -> " << typeId << endl;
    H5Aclose(attr);
    H5Tclose(type);
    return false;
  } else if (H5Tequal(type, typeId) < 0) {
    cerr << "Something went really wrong....\n\n";
    H5Aclose(attr);
    H5Tclose(type);
    return false;
  }
  hid_t status = H5Aread(attr, BasicTypeToH5<T>::memTypeId, value);
  H5Aclose(attr);
  H5Tclose(type);
  return status >= 0;
}

template bool H5Reader::attribute<int>(const string&, const string&, int&);

} // namespace tomviz
