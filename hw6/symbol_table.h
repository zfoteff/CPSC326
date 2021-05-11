//----------------------------------------------------------------------
// NAME: S. Bowers
// FILE: symbol_table.h
// DATE: Spring 2021
// DESC: Basic symbol table implementation for type checking.
//----------------------------------------------------------------------


#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <map>
#include <vector>
#include <list>
#include "data_object.h"

// string->string map to store type information for user-defined types
typedef std::map<std::string,std::string> StringMap;

// string vector (list) to store type information for functions
typedef std::vector<std::string> StringVec;


class SymbolTable
{
public:

  ~SymbolTable();

  // add a new environment to the environment stack
  void push_environment();

  // remove last added environment from the environment stack
  void pop_environment();

  // get the current environment identifier
  int get_environment_id();

  // set the current environment to the given environment identifier
  void set_environment_id(int env_id);

  // add given name to the current environment
  void add_name(const std::string& name);

  // check if name exists in current or ancestor environments
  bool name_exists(const std::string& name) const;

  // check if name exists in current environment
  bool name_exists_in_curr_env(const std::string& name) const;

  // check if name exists in given environment
  bool name_exists_in_env(const std::string& name, int env_id) const;

  // set the name's symbol-table info (as a string)
  void set_str_info(const std::string& name, const std::string& info);

  // set the name's symbol-table info (as a data object)
  void set_val_info(const std::string& name, const DataObject& info);

  // set the name's symbol-table info (as a string->string map)
  void set_map_info(const std::string& name, const StringMap& info);

  // set the nane's symbol-table info (as a vector of string)
  void set_vec_info(const std::string& name, const StringVec& info);
  
  // returns true if the name exists and has string information
  bool has_str_info(const std::string& name) const;

  // returns true if the name exists and has data-object information
  bool has_val_info(const std::string& name) const;
  
  // returns true if the name exists and has map information
  bool has_map_info(const std::string& name) const;

  // returns true if the name exists and has vector information
  bool has_vec_info(const std::string& name) const;
  
  // get the name's symbol-table info (if stored as a string)
  void get_str_info(const std::string& name, std::string& info) const;

  // get the name's symbol-table info (if stored as a data object)
  void get_val_info(const std::string& name, DataObject& info) const;

  // get the name's symbol-table info (if stored as a map)
  void get_map_info(const std::string& name, StringMap& info) const;

  // get the name's symbol-table info (if stored as a map)
  void get_vec_info(const std::string& name, StringVec& info) const;
  
  // give a string representation for printing/testing
  std::string to_string() const;
  
private:

  // a generic object for storing multiple types within the symbol table
  enum Type {STR, MAP, VEC, VAL};
  struct SymTableObject {
    virtual Type type() = 0;
  };
  struct StrObject : SymTableObject
  {
    std::string str_val;
    Type type() {return STR;};
  };
  struct ValObject : SymTableObject
  {
    DataObject obj_val;
    Type type() {return VAL;};
  };
  struct MapObject : SymTableObject
  {
    StringMap map_val;
    Type type() {return MAP;};
  };
  // a vector object
  struct VecObject : SymTableObject
  {
    StringVec vec_val;
    Type type() {return VEC;};
  };
  
  // an environment is a string to object mapping
  typedef std::map<std::string,SymTableObject*> Environment;

  // a symbol table is a stack of environment id, environment pairs
  typedef std::vector<std::pair<int,Environment>> EnvironmentList;

  // the list of environments
  EnvironmentList environments;

  // environment counter (for assignment environment ids
  int environment_count = 0;
  
  // holds the current environment identifier
  int current_environment_id = 0;

  // gets the current environment index 
  int curr_env_index() const;

  // get environment index containing the given name, starting from
  // current environment and moving up the stack of environments
  bool get_env_for_name(const std::string& name, int& index) const;

  // delete appropriate symbol table object (based on type)
  void delete_sym_obj(SymTableObject* obj);
  
};


//----------------------------------------------------------------------
// BASIC SYMBOL TABLE OPERATIONS
//----------------------------------------------------------------------

SymbolTable::~SymbolTable()
{
  for (std::pair<int,Environment> p1 : environments) {
    for (std::pair<std::string,SymTableObject*> p2 : p1.second)
      delete_sym_obj(p2.second);
    p1.second.clear();
  }
  environments.clear();
}


void SymbolTable::push_environment()
{
  std::pair<int,Environment> env_entry;
  env_entry.first = environment_count++;
  auto it = environments.begin();
  // insert as first or from current environment
  if (environments.size() == 0)
    environments.insert(it, env_entry);
  else
    environments.insert(it + curr_env_index() + 1, env_entry);
  current_environment_id = env_entry.first;
}


void SymbolTable::pop_environment()
{
  if (environments.size() == 0)
    return;
  int index = curr_env_index();
  // clean up environment
  for (std::pair<std::string,SymTableObject*> m : environments[index].second)
    delete_sym_obj(m.second);
  // remove the environment
  environments.erase(environments.begin() + index);
  if (index > 0)
    current_environment_id = environments[index-1].first;
  else
    current_environment_id = -1;
}


int SymbolTable::get_environment_id()
{
  return current_environment_id;
}


void SymbolTable::set_environment_id(int env_id)
{
  current_environment_id = env_id;
}

  
void SymbolTable::add_name(const std::string& name)
{
  if (environments.size() == 0)
    return;
  environments[curr_env_index()].second[name] = nullptr;
}


bool SymbolTable::name_exists(const std::string& name) const
{
  if (environments.size() == 0)
    return false;
  int index = 0;
  return get_env_for_name(name, index);
}

//----------------------------------------------------------------------
// SET FUNCTIONS
//----------------------------------------------------------------------

void SymbolTable::set_str_info(const std::string& name, const std::string& info)
{
  int index = -1;
  if (get_env_for_name(name, index)) {
    StrObject* obj = new StrObject;
    obj->str_val = info;
    if (environments[index].second[name])
      delete_sym_obj(environments[index].second[name]);
    environments[index].second[name] = obj;
  }
}


void SymbolTable::set_val_info(const std::string& name, const DataObject& info)
{
  int index = -1;
  if (get_env_for_name(name, index)) {
    ValObject* obj = new ValObject;
    obj->obj_val = info;
    if (environments[index].second[name])
      delete_sym_obj(environments[index].second[name]);
    environments[index].second[name] = obj;
  }
}


void SymbolTable::set_vec_info(const std::string& name, const StringVec& info)
{
  int index = -1;
  if (get_env_for_name(name, index)) {
    VecObject* obj = new VecObject;
    obj->vec_val = info;
    if (environments[index].second[name])
      delete_sym_obj(environments[index].second[name]);
    environments[index].second[name] = obj;
  }
}


void SymbolTable::set_map_info(const std::string& name, const StringMap& info)
{
  int index = -1;
  if (get_env_for_name(name, index)) {
    MapObject* obj = new MapObject;
    obj->map_val = info;
    if (environments[index].second[name])
      delete_sym_obj(environments[index].second[name]);
    environments[index].second[name] = obj;
  }
}


//----------------------------------------------------------------------
// HAS FUNCTIONS
//----------------------------------------------------------------------

bool SymbolTable::has_str_info(const std::string& name) const
{
  int index = -1;
  if (get_env_for_name(name, index)) {
    SymTableObject* obj = environments[index].second.at(name);
    return obj and obj->type() == STR;
  }
  return false;
}


bool SymbolTable::has_val_info(const std::string& name) const
{
  int index = -1;
  if (get_env_for_name(name, index)) {
    SymTableObject* obj = environments[index].second.at(name);
    return obj and obj->type() == VAL;
  }
  return false;
}
  

bool SymbolTable::has_vec_info(const std::string& name) const
{
  int index = -1;
  if (get_env_for_name(name, index)) {
    SymTableObject* obj = environments[index].second.at(name);
    return obj and obj->type() == VEC;
  }
  return false;
}


bool SymbolTable::has_map_info(const std::string& name) const
{
  int index = -1;
  if (get_env_for_name(name, index)) {
    SymTableObject* obj = environments[index].second.at(name);
    return obj and obj->type() == MAP;
  }
  return false;
}


//----------------------------------------------------------------------
// GET FUNCTIONS
//----------------------------------------------------------------------

void SymbolTable::get_str_info(const std::string& name, std::string& info) const
{
  int index = -1;
  if (get_env_for_name(name, index)) {
    SymTableObject* obj = environments[index].second.at(name);
    if (obj)
      info = ((StrObject*)obj)->str_val;
  }
}


void SymbolTable::get_val_info(const std::string& name, DataObject& info) const
{
  int index = -1;
  if (get_env_for_name(name, index)) {
    SymTableObject* obj = environments[index].second.at(name);
    if (obj) 
      info = ((ValObject*)obj)->obj_val;
  }
}


void SymbolTable::get_vec_info(const std::string& name, StringVec& info) const
{
  int index = -1;
  if (get_env_for_name(name, index)) {
    SymTableObject* obj = environments[index].second.at(name);
    if (obj)
      info = ((VecObject*)obj)->vec_val;
  }
}


void SymbolTable::get_map_info(const std::string& name, StringMap& info) const
{
  int index = -1;
  if (get_env_for_name(name, index)) {
    SymTableObject* obj = environments[index].second.at(name);
    if (obj)
      info = ((MapObject*)obj)->map_val;
  }
}


//----------------------------------------------------------------------
// PRETTY PRINT ENVIRONMENTS
//----------------------------------------------------------------------

std::string SymbolTable::to_string() const
{
  std::string s = "";
  for (std::pair<int,Environment> env_entry : environments) {
    s += "environment " + std::to_string(env_entry.first) + ": \n";
    for (std::pair<std::string,SymTableObject*> p : env_entry.second) {
      s += "  name '" + p.first + "' has-info ";
      if (p.second) {
        if (p.second->type() == STR)
          s += "STR '" + ((StrObject*)p.second)->str_val + "'";
        if (p.second->type() == VAL)
          s += "VAL '" + ((ValObject*)p.second)->obj_val.to_string() + "'";
        else if (p.second->type() == VEC) {
          s += "StringVec {";
          StringVec vec = ((VecObject*)p.second)->vec_val;
          int i = 0;
          for (std::string val : vec) {
            s += "'" + val + "'";
            if (i < vec.size() -1)
              s += ", ";
            ++i;
          }
          s += "}";
        }
        else if (p.second->type() == MAP) {
          s += "StringMap {";
          StringMap map = ((MapObject*)p.second)->map_val;
          int i = 0;
          for (std::pair<std::string,std::string> p : map) {
            s += "'" + p.first + "': '" + p.second + "'";
            if (i < map.size() -1)
              s += ", ";
            ++i;
          }
          s += "}";
        }
      }
      s += "\n";
    }
  }
  return s;
}



//----------------------------------------------------------------------
// HELPER FUNCTIONS
//----------------------------------------------------------------------

void SymbolTable::delete_sym_obj(SymTableObject* obj)
{
  if (!obj)
    return;
  else if (obj->type() == STR)
    delete ((StrObject*)obj);
  else if (obj->type() == MAP)
    delete ((MapObject*)obj);
  else if (obj->type() == VEC)
    delete ((VecObject*)obj);
  else if (obj->type() == VAL)
    delete ((ValObject*)obj);
}


bool SymbolTable::name_exists_in_curr_env(const std::string& name) const
{
  return name_exists_in_env(name, current_environment_id);
}


bool SymbolTable::name_exists_in_env(const std::string& name, int env_id) const
{
  for (std::pair<int,Environment> env_entry : environments) {
    if (env_entry.first == env_id)
      return env_entry.second.count(name) > 0;
  }
  return false;
}


bool SymbolTable::get_env_for_name(const std::string& name, int& index) const
{
  int curr_index = curr_env_index();
  for (size_t i = curr_index + 1; i > 0; --i) {
    if (environments[i-1].second.count(name) > 0) {
      index = i - 1;
      return true;
    }
  }
  return false;
}

  
int SymbolTable::curr_env_index() const
{
  for (int i = 0; i < environments.size(); ++i) {
    if (current_environment_id == environments[i].first)
      return i;
  }
  return -1;
}


#endif
