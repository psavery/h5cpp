/* This source file is part of the Tomviz project, https://tomviz.org/.
   It is released under the 3-Clause BSD License, see "LICENSE". */

#ifndef tomvizH5AttributeReader_h
#define tomvizH5AttributeReader_h

namespace tomviz {

class H5AttributeReader()
{
public:
  explicit H5AttributeReader(hid_t fileId, const std::string& group,
                             const std::string& name)
  {
    m_attr = H5Aopen_by_name(fileId, group.c_str(), name.c_str(),
                             H5P_DEFAULT, H5P_DEFAULT);
  }

  ~H5AttributeReader()
  {
    H5Aclose(m_attr);
  }

  hid_t attr() { return m_attr; }

private:
  hid_t m_attr;
};

} // namespace tomviz

#endif // tomvizH5Attribute
