//----------------------------------------------------------------------
// Name: S. Bowers
// File: data_values.h
// Date: Spring 2021
// Desc: For representing MyPL basic data values during
//       interpretation. A DataType is essentially a container for a
//       primitive value that can be set (modified) and retrieved.
//----------------------------------------------------------------------


#ifndef DATA_OBJECT_H
#define DATA_OBJECT_H

#include <string>



class DataObject
{
public:

  enum DataType {INTEGER, DOUBLE, STRING, CHAR, BOOL, OID, NIL};

  // construction
  DataObject();
  DataObject(int val);
  DataObject(double val);
  DataObject(const char* val);
  DataObject(const std::string& val);
  DataObject(char val);
  DataObject(bool val);
  DataObject(size_t val);
  // destruction
  ~DataObject();
  // copying
  DataObject(const DataObject& rhs);
  DataObject& operator=(const DataObject& rhs);
  // set/update
  void set(int val);
  void set(double val);
  void set(const char* val);
  void set(const std::string& val);
  void set(char val);
  void set(bool val);
  void set(size_t val);
  void set_nil();
  // get and check type
  DataType type() const;
  bool is_nil() const;
  bool is_integer() const;
  bool is_double() const;
  bool is_string() const;
  bool is_char() const;
  bool is_bool() const;
  bool is_oid() const;
  // get the value
  bool value(int& val) const;
  bool value(double& val) const;
  bool value(std::string& val) const;
  bool value(char& val) const;
  bool value(bool& val) const;
  bool value(size_t& val) const;
  // get a string representation
  std::string to_string() const;
 private:
  void* value_ptr = nullptr;
  DataType value_type = DataType::NIL;
  void delete_obj();
};

//----------------------------------------------------------------------
// CONSTRUCTION
//----------------------------------------------------------------------

DataObject::DataObject()
{
  set_nil();
}

DataObject::DataObject(int val)
{
  set(val);
}

DataObject::DataObject(double val)
{
  set(val);
}

DataObject::DataObject(const char* val)
{
  set(std::string(val));
}

DataObject::DataObject(const std::string& val)
{
  set(val);
}

DataObject::DataObject(char val)
{
  set(val);
}

DataObject::DataObject(bool val)
{
  set(val);
}

DataObject::DataObject(size_t val)
{
  set(val);
}


//----------------------------------------------------------------------
// DESTRUCTION
//----------------------------------------------------------------------
void DataObject::delete_obj()
{
  if (value_type == DataType::INTEGER)
    delete (int*)value_ptr;
  else if (value_type == DataType::DOUBLE)
    delete (double*)value_ptr;
  else if (value_type == DataType::STRING)
    delete (std::string*)value_ptr;
  else if (value_type == DataType::CHAR)
    delete (char*)value_ptr;
  else if (value_type == DataType::BOOL)
    delete (bool*)value_ptr;
  else if (value_type == DataType::OID)
    delete (size_t*)value_ptr;
}

DataObject::~DataObject()
{
  delete_obj();
}


//----------------------------------------------------------------------
// COPYING
//----------------------------------------------------------------------

DataObject::DataObject(const DataObject& rhs)
{
  *this = rhs;
}

DataObject& DataObject::operator=(const DataObject& rhs)
{
  if (this == &rhs)
    return *this;
  if (rhs.is_integer()) {
    int v;
    rhs.value(v);
    set(v);
  }
  else if (rhs.is_double()) {
    double v;
    rhs.value(v);
    set(v);
  }
  else if (rhs.is_string()) {
    std::string v;
    rhs.value(v);
    set(v);
  }
  else if (rhs.is_char()) {
    char v;
    rhs.value(v);
    set(v);
  }
  else if (rhs.is_bool()) {
    bool v;
    rhs.value(v);
    set(v);
  }
  else if (rhs.is_oid()) {
    size_t v;
    rhs.value(v);
    set(v);
  }
  else if (rhs.is_nil()) {
    set_nil();
  }
  return *this;
}


//----------------------------------------------------------------------
// SET/UPDATE
//----------------------------------------------------------------------

void DataObject::set(int val)
{
  delete_obj();
  value_ptr = new int;
  *((int*)value_ptr) = val;
  value_type = DataType::INTEGER;
}

void DataObject::set(double val)
{
  delete_obj();
  value_ptr = new double;
  *((double*)value_ptr) = val;
  value_type = DataType::DOUBLE;
}

void DataObject::set(const char* val)
{
  delete_obj();
  value_ptr = new std::string;
  *((std::string*)value_ptr) = val;
  value_type = DataType::STRING;
}

void DataObject::set(const std::string& val)
{
  delete_obj();
  value_ptr = new std::string;
  *((std::string*)value_ptr) = val;
  value_type = DataType::STRING;
}

void DataObject::set(char val)
{
  delete_obj();
  value_ptr = new char;
  *((char*)value_ptr) = val;
  value_type = DataType::CHAR;
}

void DataObject::set(bool val)
{
  delete_obj();
  value_ptr = new bool;
  *((bool*)value_ptr) = val;
  value_type = DataType::BOOL;
}

void DataObject::set(size_t val)
{
  delete_obj();
  value_ptr = new size_t;
  *((size_t*)value_ptr) = val;
  value_type = DataType::OID;
}

void DataObject::set_nil()
{
  delete_obj();
  value_ptr = nullptr;
  value_type = DataType::NIL;
}


//----------------------------------------------------------------------
// GET TYPE
//----------------------------------------------------------------------

DataObject::DataType DataObject::type() const
{
  return value_type;
}

bool DataObject::is_nil() const
{
  return type() == DataType::NIL;
}

bool DataObject::is_integer() const
{
  return type() == DataType::INTEGER;
}

bool DataObject::is_double() const
{
  return type() == DataType::DOUBLE;
}

bool DataObject::is_string() const
{
  return type() == DataType::STRING;
}

bool DataObject::is_char() const
{
  return type() == DataType::CHAR;
}

bool DataObject::is_bool() const
{
  return type() == DataType::BOOL;
}

bool DataObject::is_oid() const
{
  return type() == DataType::OID;
}


//----------------------------------------------------------------------
// GET THE VALUE
//----------------------------------------------------------------------

bool DataObject::value(int& val) const
{
  if (value_type != DataType::INTEGER or !value_ptr)
    return false;
  val = *((int*)value_ptr);
  return true;
}

bool DataObject::value(double& val) const
{
  if (value_type != DataType::DOUBLE or !value_ptr)
    return false;
  val = *((double*)value_ptr);
  return true;
}

bool DataObject::value(std::string& val) const
{
  if (value_type != DataType::STRING or !value_ptr)
    return false;
  val = *((std::string*)value_ptr);
  return true;
}

bool DataObject::value(char& val) const
{
  if (value_type != DataType::CHAR or !value_ptr)
    return false;
  val = *((char*)value_ptr);
  return true;
}

bool DataObject::value(bool& val) const
{
  if (value_type != DataType::BOOL or !value_ptr)
    return false;
  val = *((bool*)value_ptr);
  return true;
}

bool DataObject::value(size_t& val) const
{
  if (value_type != DataType::OID or !value_ptr)
    return false;
  val = *((size_t*)value_ptr);
  return true;
}


//----------------------------------------------------------------------
// GET A STRING REPRESENTATION
//----------------------------------------------------------------------

std::string DataObject::to_string() const
{
  if (!value_ptr or value_type == DataType::NIL)
    return "";
  else if (value_type == DataType::INTEGER)
    return std::to_string(*((int*)value_ptr));
  else if (value_type == DataType::DOUBLE)
    return std::to_string(*((double*)value_ptr));
  else if (value_type == DataType::STRING)
    return *((std::string*)value_ptr);
  else if (value_type == DataType::CHAR)
    return std::to_string(*((char*)value_ptr));
  else if (value_type == DataType::BOOL)
    return std::to_string(*((bool*)value_ptr));
  else if (value_type == DataType::OID)
    return std::to_string(*((size_t*)value_ptr));
  else
    return "";
}



#endif
