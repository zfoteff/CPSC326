//----------------------------------------------------------------------
// Name: S. Bowers
// File: heap.h
// Date: Spring 2021
// Desc: Basic Heap implementation for the MyPL Interpreter. The Heap
//       is used to store objects of user-defined types. Each object
//       has a unique object id (OID) and a collection of key-value
//       pairs. The keys denote user-defined type variable names and
//       the values denote the corresponding variable values. Each
//       value is represented as a DataObject. The key-value pairs are
//       represented as HeapObjects.
//----------------------------------------------------------------------

#ifndef HEAP_H
#define HEAP_H

#include <unordered_map>
#include "data_object.h"


class HeapObject
{
public:

  //----------------------------------------------------------------------
  // Add or update a given name with the given data object.
  // Inputs:
  //   att -- the attribute (variable) name
  //   obj -- the attribute (variable) value
  //----------------------------------------------------------------------
  void set_att(const std::string& att, const DataObject& obj);

  //----------------------------------------------------------------------
  // Check if the attribute exists in the heap object
  // Inputs:
  //   att -- the attribute (variable) to check
  // Returns:
  //   true if the attribute has been added, false otherwise
  //----------------------------------------------------------------------
  bool has_att(const std::string& att) const;

  //----------------------------------------------------------------------
  // Get the value of the given attribute
  // Inputs:
  //   att -- the attribute to get the value of
  // Outputs:
  //   obj -- the value of the object
  // Returns:
  //   true if the heap object has the given attribute defined
  //----------------------------------------------------------------------
  bool get_val(const std::string& att, DataObject& val);  

private:
  std::unordered_map<std::string,DataObject> attribute_values;
};


class Heap
{
public:

  //----------------------------------------------------------------------
  // Add or update the oid with the given heap object.
  // Inputs:
  //   oid -- the oid to add or update
  //   obj -- the value of the oid
  //----------------------------------------------------------------------
  void set_obj(size_t oid, const HeapObject& obj);

  //----------------------------------------------------------------------
  // Check if the oid is in the heap.
  // Inputs:
  //   oid -- the oid to check for
  // Returns:
  //   true if the oid is present in the heap, false otherwise
  //----------------------------------------------------------------------
  bool has_obj(size_t oid) const;

  //----------------------------------------------------------------------
  // Get the user-defined type object associated with given oid.
  // Inputs:
  //   oid -- the oid to look up
  // Outputs:
  //   obj -- the heap object associated with the oid
  // Returns:
  //   true if the oid is present in the heap, and false otherwise
  //----------------------------------------------------------------------
  bool get_obj(size_t oid, HeapObject& obj) const;

private:
  std::unordered_map<size_t, HeapObject> heap_objs;
};


//----------------------------------------------------------------------
// HeapObject Member Functions
//----------------------------------------------------------------------

void HeapObject::set_att(const std::string& att, const DataObject& obj)
{
  attribute_values[att] = obj;
}

bool HeapObject::has_att(const std::string& att) const
{
  return attribute_values.count(att) > 0;
}

bool HeapObject::get_val(const std::string& att, DataObject& val)
{
  if (!has_att(att))
    return false;
  val = attribute_values.at(att);
  return true;
}


//----------------------------------------------------------------------
// Heap Member Functions
//----------------------------------------------------------------------

void Heap::set_obj(size_t oid, const HeapObject& obj)
{
  heap_objs[oid] = obj;
}


bool Heap::has_obj(size_t oid) const
{
  return heap_objs.count(oid) > 0;
}


bool Heap::get_obj(size_t oid, HeapObject& obj) const
{
  if (!has_obj(oid))
    return false;
  obj = heap_objs.at(oid);
  return true;
}


#endif
