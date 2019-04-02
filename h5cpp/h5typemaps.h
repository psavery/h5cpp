/* This source file is part of the Tomviz project, https://tomviz.org/.
   It is released under the 3-Clause BSD License, see "LICENSE". */

#ifndef tomvizH5TypeMaps_h
#define tomvizH5TypeMaps_h

#include "h5capi.h"

template<typename T>
struct BasicTypeToH5;

// Because these H5T_* macros are actually functions,
// we have to retrieve them like this, or we get
// compile errors.
template<>
struct BasicTypeToH5<char>
{
  static hid_t dataTypeId() { return H5T_STD_I8LE; }
  static hid_t memTypeId() { return H5T_NATIVE_CHAR; }
};

template<>
struct BasicTypeToH5<unsigned char>
{
  static hid_t dataTypeId() { return H5T_STD_U8LE; }
  static hid_t memTypeId() { return H5T_NATIVE_UCHAR; }
};

template<>
struct BasicTypeToH5<int>
{
  static hid_t dataTypeId() { return H5T_STD_I32LE; }
  static hid_t memTypeId() { return H5T_NATIVE_INT; }
};

template<>
struct BasicTypeToH5<short>
{
  static hid_t dataTypeId() { return H5T_STD_I16LE; }
  static hid_t memTypeId() { return H5T_NATIVE_SHORT; }
};

template<>
struct BasicTypeToH5<long long>
{
  static hid_t dataTypeId() { return H5T_STD_I64LE; }
  static hid_t memTypeId() { return H5T_NATIVE_LLONG; }
};

template<>
struct BasicTypeToH5<unsigned int>
{
  static hid_t dataTypeId() { return H5T_STD_U32LE; }
  static hid_t memTypeId() { return H5T_NATIVE_UINT; }
};

template<>
struct BasicTypeToH5<unsigned short>
{
  static hid_t dataTypeId() { return H5T_STD_U16LE; }
  static hid_t memTypeId() { return H5T_NATIVE_USHORT; }
};

template<>
struct BasicTypeToH5<unsigned long long>
{
  static hid_t dataTypeId() { return H5T_STD_U64LE; }
  static hid_t memTypeId() { return H5T_NATIVE_ULLONG; }
};

template<>
struct BasicTypeToH5<float>
{
  static hid_t dataTypeId() { return H5T_IEEE_F32LE; }
  static hid_t memTypeId()  { return H5T_NATIVE_FLOAT; }
};

template<>
struct BasicTypeToH5<double>
{
  static hid_t dataTypeId() { return H5T_IEEE_F64LE; }
  static hid_t memTypeId() { return H5T_NATIVE_DOUBLE; }
};

#endif // tomvizH5TypeMaps
