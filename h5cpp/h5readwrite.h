/* This source file is part of the Tomviz project, https://tomviz.org/.
   It is released under the 3-Clause BSD License, see "LICENSE". */

#ifndef tomvizH5ReadWrite_h
#define tomvizH5ReadWrite_h

#include <memory>
#include <string>
#include <vector>

namespace h5 {

class H5ReadWrite {
public:

  /**
   * Enumeration of the open modes.
   */
  enum class OpenMode {
    ReadOnly,
    WriteOnly
  };

  /**
   * Open an HDF5 file for reading.
   * @param fileName the file to open for reading.
   */
  explicit H5ReadWrite(const std::string& fileName,
                    OpenMode mode = OpenMode::ReadOnly);

  /** Closes the file and destroys the H5ReadWrite */
  ~H5ReadWrite();

  /** Copy constructor is disabled */
  H5ReadWrite(const H5ReadWrite&) = delete;

  /** Assignment operator is disabled */
  H5ReadWrite& operator=(const H5ReadWrite&) = delete;

  /** Enumeration of the data types */
  enum class DataType {
    Int8,
    Int16,
    Int32,
    Int64,
    UInt8,
    UInt16,
    UInt32,
    UInt64,
    Float,
    Double,
    String,
    None = -1
  };

  /** Get a string representation of the enum DataType */
  static std::string dataTypeToString(const DataType& type);

  /**
   * Get the children of a path.
   * @param ok If used, set to true on success and false on failure.
   * @return A vector of the names of the children.
   */
  std::vector<std::string> children(const std::string& path,
                                    bool* ok = nullptr);

  /**
   * Check if a given path has at least one attribute.
   * @param path The path to the attribute.
   * @return True if the path has an attribute, false if it does not,
   *         or if an error occurred.
   */
  bool hasAttribute(const std::string& path);

  /**
   * Check if a given path has an attribute with a given name.
   * @param path The path to the attribute.
   * @param name The name of the attribute.
   * @return True if the path has the attribute, false if it does not,
   *         or if an error occurred.
   */
  bool hasAttribute(const std::string& path, const std::string& name);

  /**
   * Get an attribute's type.
   * @param path The path to the attribute.
   * @param name The name of the attribute.
   * @return The datatype of the attribute, or DataType::None on failure.
   */
  DataType attributeType(const std::string& path, const std::string& name);

  /**
   * Read an attribute and interpret it as type T. If T is not
   * the correct type of the attribute, an error will occur.
   * @param path The path to the attribute.
   * @param name The name of the attribute.
   * @ok If used, set to true on success and false on failure.
   * @return The attribute.
   */
  template <typename T>
  T attribute(const std::string& path, const std::string& name,
              bool* ok = nullptr);

  /**
   * Check if a given path is a data set.
   * @return True if the path is a data set, false if it is not, or if
   *         an error occurred.
   */
  bool isDataSet(const std::string& path);

  /**
   * Get the paths to all of the data sets in the file.
   * This could potentially be an expensive operation for large files.
   * @return A vector of strings of the paths to the data sets.
   */
  std::vector<std::string> allDataSets();

  /**
   * Get a data set's type. An error will occur if @p path is not a dataset.
   * @param path The path to the data set.
   * @return The datatype of the attribute, or DataType::None on failure.
   */
  DataType dataType(const std::string& path);

  /**
   * Get the number of dimensions of a data set.
   * @param path The path to the data set.
   * @return The number of dimensions, or a negative number on failure.
   */
  int dimensionCount(const std::string& path);

  /**
   * Get the dimensions of a data set.
   * @param path The path to the data set.
   * @return A vector of the dimensions, or an empty vector on failure.
   */
  std::vector<int> getDimensions(const std::string& path);

  /**
   * Read a 1-dimensional data set and interpret it as type T. If @p path
   * is not a data set, @p path is not a 1-dimensional data set, or T is
   * not the correct type of the data set, an error will occur.
   * @param path The path to the data set.
   * @return A vector of the data, or an empty vector on failure.
   */
  template <typename T>
  std::vector<T> readData(const std::string& path);

  /**
   * Read a multi-dimensional data set and interpret it as type T. If
   * @p path is not a data set, or T is not the correct type of the
   * data set, an error will occur.
   * @param path The path to the data set.
   * @param dimensions Will be set to the dimensions of the data set.
   * @return A vector of the data, or an empty vector on failure.
   */
  template <typename T>
  std::vector<T> readData(const std::string& path,
                          std::vector<int>& dimensions);

  /**
   * Read a multi-dimensional data set and interpret it as type T. If
   * @p path is not a data set, or T is not the correct type of the
   * data set, an error will occur.
   * @param path The path to the data set.
   * @param data A pointer to a block of memory with a size large enough
   *             to hold the data (size >= dim1 * dim2 * dim3...). This
   *             will be set to the data read from the data set.
   * @return True on success, false on failure.
   */
  template <typename T>
  bool readData(const std::string& path, T* data);


  /**
   * Write data to a specified path.
   * @param path The path where the data will be written.
   * @param name The name of the data.
   * @param dimensions The dimensions of the data.
   * @param data The data to write.
   * @return True on success, false on failure.
   */
  template <typename T>
  bool writeData(const std::string& path, const std::string& name,
                 const std::vector<int>& dimensions,
                 const std::vector<T>& data);

  /**
   * Set an attribute on a specified path.
   * @param path The path where the attribute will be written.
   * @param name The name of the attribute.
   * @param value The value of the attribute.
   * @return True on success, false on failure.
   */
  template <typename T>
  bool setAttribute(const std::string& path, const std::string& name, T value);

private:
  class H5ReadWriteImpl;
  std::unique_ptr<H5ReadWriteImpl> m_impl;
};

} // namespace h5

#endif // tomvizH5ReadWrite_h
