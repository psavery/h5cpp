/* This source file is part of the Tomviz project, https://tomviz.org/.
   It is released under the 3-Clause BSD License, see "LICENSE". */

#ifndef tomvizH5AttributeReader_h
#define tomvizH5AttributeReader_h

#include <string>

#include "h5capi.h"
#include "h5typereader.h"

namespace tomviz {

class H5AttributeReader
{
public:
  explicit H5AttributeReader(hid_t fileId, const std::string& group,
                             const std::string& name)
  {
    m_attr = H5Aopen_by_name(fileId, group.c_str(), name.c_str(),
                             H5P_DEFAULT, H5P_DEFAULT);
  }

  hid_t attr() { return m_attr; }

  H5TypeReader type() { return H5TypeReader(H5Aget_type(m_attr)); }

  ~H5AttributeReader()
  {
    clear();
  }

  H5AttributeReader(const H5AttributeReader&) = delete;
  H5AttributeReader& operator=(const H5AttributeReader&) = delete;

  H5AttributeReader(H5AttributeReader&& other)
  {
    *this = std::move(other);
  }

  H5AttributeReader& operator=(H5AttributeReader&& other)
  {
    clear();
    m_attr = other.m_attr;
    other.m_attr = H5I_INVALID_HID;
  }

  void clear()
  {
    if (m_attr > 0) {
      H5Aclose(m_attr);
      m_attr = H5I_INVALID_HID;
    }
  }

private:
  hid_t m_attr;
};

} // namespace tomviz

#endif // tomvizH5AttributeReader
