/* This source file is part of the Tomviz project, https://tomviz.org/.
   It is released under the 3-Clause BSD License, see "LICENSE". */

#ifndef tomvizH5TypeReader_h
#define tomvizH5TypeReader_h

#include "h5capi.h"

namespace tomviz {

class H5TypeReader
{
public:
  explicit H5TypeReader(hid_t type)
    : m_type(type)
  {
  }

  hid_t type() { return m_type; }

  H5TypeReader(const H5TypeReader&) = delete;
  H5TypeReader& operator=(const H5TypeReader&) = delete;

  H5TypeReader(H5TypeReader&& other) noexcept
  {
    *this = std::move(other);
  }

  H5TypeReader& operator=(H5TypeReader&& other) noexcept
  {
    clear();
    m_type = other.m_type;
    other.m_type = H5I_INVALID_HID;
  }

  ~H5TypeReader()
  {
    clear();
  }

  void clear()
  {
    if (m_type > 0) {
      H5Tclose(m_type);
      m_type = H5I_INVALID_HID;
    }
  }

private:
  hid_t m_type = H5I_INVALID_HID;
};

} // namespace tomviz

#endif // tomvizH5TypeReader
