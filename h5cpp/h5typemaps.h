/* This source file is part of the Tomviz project, https://tomviz.org/.
   It is released under the 3-Clause BSD License, see "LICENSE". */

#ifndef tomvizH5TypeMaps_h
#define tomvizH5TypeMaps_h

#include "h5capi.h"

template<typename T>
struct BasicTypeToH5;

template<>
struct BasicTypeToH5<char>
{
  static const hid_t dataTypeId = H5T_STD_I8LE;
  static const hid_t memTypeId = H5T_NATIVE_CHAR;
};

template<>
struct BasicTypeToH5<unsigned char>
{
  static const hid_t dataTypeId = H5T_STD_U8LE;
  static const hid_t memTypeId = H5T_NATIVE_UCHAR;
};

template<>
struct BasicTypeToH5<int>
{
  static const hid_t dataTypeId = H5T_STD_I32LE;
  static const hid_t memTypeId = H5T_NATIVE_INT;
};

template<>
struct BasicTypeToH5<short>
{
  static const hid_t dataTypeId = H5T_STD_I16LE;
  static const hid_t memTypeId = H5T_NATIVE_SHORT;
};

template<>
struct BasicTypeToH5<long long>
{
  static const hid_t dataTypeId = H5T_STD_I64LE;
  static const hid_t memTypeId = H5T_NATIVE_LLONG;
};

template<>
struct BasicTypeToH5<unsigned int>
{
  static const hid_t dataTypeId = H5T_STD_U32LE;
  static const hid_t memTypeId = H5T_NATIVE_UINT;
};

template<>
struct BasicTypeToH5<unsigned short>
{
  static const hid_t dataTypeId = H5T_STD_U16LE;
  static const hid_t memTypeId = H5T_NATIVE_USHORT;
};

template<>
struct BasicTypeToH5<unsigned long long>
{
  static const hid_t dataTypeId = H5T_STD_U64LE;
  static const hid_t memTypeId = H5T_NATIVE_ULLONG;
};

template<>
struct BasicTypeToH5<float>
{
  static const hid_t dataTypeId = H5T_IEEE_F32LE;
  static const hid_t memTypeId = H5T_NATIVE_FLOAT;
};

template<>
struct BasicTypeToH5<double>
{
  static const hid_t dataTypeId = H5T_IEEE_F64LE;
  static const hid_t memTypeId = H5T_NATIVE_DOUBLE;
};

#endif // tomvizH5TypeMaps
