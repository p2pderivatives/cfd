'use strict';
const fs = require('fs');
const path = require('path');
// FIXME(k-matsuzawa): Consider splitting the file.

// ----------------------------------------------------------------------------
// debug log function
// ----------------------------------------------------------------------------
let debugLog = function() {
  // do nothing
}

// ----------------------------------------------------------------------------
// json data class
// ----------------------------------------------------------------------------
class JsonMappingData {
  constructor(name, type, init_value, class_name, is_output_struct = true) {
    this.name = name;
    this.method_name = (() => {
      const replaced_method_name = this.name.replace(/-/gi, '_');
      return replaced_method_name.charAt(0).toUpperCase() + replaced_method_name.slice(1);
    })();
    this.variable_name = (() => {
      const replaced_variable_name = this.name.replace(/-/gi, '_');
      return replaced_variable_name.split(/(?=[A-Z])/).join('_').toLowerCase();
    })();
    this.setType(type);
    this.init_value = init_value;
    this.class_name = class_name;
    this.child_list = {};
    this.parent = null;
    this.is_output_struct = is_output_struct;
    this.is_array = false;
    this.is_object = false;
    // Reserved word support.
    // TODO(k-matsuzawa): If the number increases, make a list.
    if (this.variable_name == 'asm') this.variable_name = `${this.variable_name}_`;
  }

  setType(type) {
    this.type = type;
    this.struct_type = `${type}Struct`;
    if (type.startsWith('JsonValueVector')) {
      const type_name = type.split('<')[1].split('>')[0];
      this.struct_type = `std::vector<${type_name}>`;
    } else if (type.startsWith('JsonObjectVector')) {
      const type_name = type.split('<')[1].split(',')[0].split('>')[0];
      this.struct_type = `std::vector<${type_name}Struct>`;
    }
  }

  setTypeStruct(type, struct_type) {
    this.type = type;
    this.struct_type = struct_type;
    if (type.startsWith('JsonValueVector')) {
      const type_name = type.split('<')[1].split('>')[0];
      this.struct_type = `std::vector<${type_name}>`;
    } else if (type.startsWith('JsonObjectVector')) {
      const type_name = type.split('<')[1].split(',')[0].split('>')[0];
      this.struct_type = `std::vector<${type_name}Struct>`;
    }
  }

  toString() {
    const str = `[JsonMappingData] ${this.name}:${this.type}:${this.class_name}`;
    // for debug code.
    // for (const key in this.child_list) {
    //   str += "\n - "
    //   str += this.child_list[key].toString()
    // }
    return str;
  }

  collectMapData(map, list) {
    if (this.type.startsWith('JsonValueVector') || this.type.startsWith('JsonObjectVector')) {
      for (const key in this.child_list) {
        if (this.child_list[key]) {
          const ret = this.child_list[key].collectMapData(map, list);
          return {type: ret['type'] + '[]', map: ret['map'], list: ret['list']};
        }
        break;
      }
      throw Error('Illegal state.');
    } else if (this.type === 'ErrorResponseBase') {
      const clsName = 'ErrorResponse';
      let props = [];
      let tmpMap = map;
      let tmpList = list;
      for (const key in this.child_list) {
        if (this.child_list[key]) {
          const name = this.child_list[key].name;
          const ret = this.child_list[key].collectMapData(tmpMap, tmpList);
          const type = ret['type'];
          tmpMap = ret['map'];
          tmpList = ret['list'];
          if (name === 'isOutputStruct') {
            continue;
          }
          props.push({name: name, type: type});
        }
      }
      tmpMap[clsName] = props;
      tmpList.push(clsName);
      return {type: clsName, map: tmpMap, list: tmpList};
    } else if (Object.keys(this.child_list).length > 0) {
      // my class name
      let props = [];
      let tmpMap = map;
      let tmpList = list;
      for (const key in this.child_list) {
        if (this.child_list[key]) {
          let name = this.child_list[key].name;
          const ret = this.child_list[key].collectMapData(tmpMap, tmpList);
          debugLog('prop : ', ret);
          const type = ret['type'];
          tmpMap = ret['map'];
          tmpList = ret['list'];
          if (name.indexOf('-') > 0) {
            name = "'" + name + "'";
          }
          props.push({name: name, type: type});
        }
      }
      debugLog("props = ", props);
      tmpMap[this.type] = props;
      tmpList.push(this.type);
      return {type: this.type, map: tmpMap, list: tmpList};
    } else {
      let type = '';
      if (this.type === 'std::string') {
        type = 'string';
      } else if (this.type === 'bool') {
        type = 'boolean';
      } else if ((this.type === 'int64_t') || (this.type === 'uint64_t')) {
        type = 'bigint';
      } else {
        type = 'number';
      }
      return {type: type, map: map, list: list};
    }
  }

  getFunctionName() {
    let result = '';
    if (this.type.indexOf('Request') >= 0) {
      result = this.type.substring(0, this.type.indexOf('Request'));
    } else if (this.type.indexOf('Response') >= 0) {
      result = this.type.substring(0, this.type.indexOf('Response'));
    }
    // ignore target
    if (result === 'Error') {
      return '';
    }
    return result;
  }
}

// ----------------------------------------------------------------------------
// json data class
// ----------------------------------------------------------------------------
class JsonData {
  constructor(json_data, request_data, response_data) {
    this.json_data = json_data;
    this.request_data = request_data;
    this.response_data = response_data;
  }
}

// ----------------------------------------------------------------------------
// array check function
// ----------------------------------------------------------------------------
function isArray(obj) {
  return Object.prototype.toString.call(obj) === '[object Array]';
}

// ----------------------------------------------------------------------------
// analyze function
// ----------------------------------------------------------------------------
const analyzeJson = (jsonObj, objName = '') => {
  debugLog(`analyzeJson obj=${objName}`)
  let result;
  if (typeof jsonObj == 'string') {
    result = new JsonMappingData(objName, 'std::string', jsonObj, '');
  } else if (typeof jsonObj == 'number') {
    result = new JsonMappingData(objName, 'int64_t', jsonObj, '');
  } else if (typeof jsonObj == 'boolean') {
    result = new JsonMappingData(objName, 'boolean', jsonObj, '');
  } else if (jsonObj) {
    const obj_key = Object.keys(jsonObj);
    const obj_values = Object.values(jsonObj);
    if (obj_key == 0) { // array
      debugLog(`read arr = ${obj_values}`)
      let past_type = '';
      let first_map = null;
      for (const item in jsonObj) {
        const temp_child = analyzeJson(jsonObj[item], objName);
        if (past_type == '') {
          first_map = temp_child;
          past_type = temp_child.type;
        } else if (past_type != temp_child.type) {
          console.log('illegal list elements. fail.');
          return result;
        }
      }
      debugLog(`past_type = ${past_type}`)
      if (past_type == '') {
        // field and class name is set by the caller.
        result = new JsonMappingData('', '', '', '');
      } else {
        if ((typeof obj_values[0] == 'string') || (typeof obj_values[0] == 'number') ||
          (typeof obj_values[0] == 'boolean')) {
          // array of string or number.
          result = new JsonMappingData(objName, `JsonValueVector<${past_type}>`, '', '');
        } else {
          // array of object
          result = new JsonMappingData(objName, `JsonObjectVector<${past_type}, ${past_type}Struct>`, '', '');
        }
      }
      result.child_list[0] = first_map;
      result.is_array = true;
      debugLog(`list_type = ${result.type}`)
      debugLog(`clist_type = ${result.child_list[0].type}`)
    } else { // object
      debugLog(`read keys = ${obj_key}`)
      let class_name = objName;
      if (':class' in jsonObj) {
        class_name = jsonObj[':class'];
        debugLog(`read class_name = ${class_name}`)
      }
      let is_output_struct = true;
      if (':isOutputStruct' in jsonObj) {
        is_output_struct = jsonObj[':isOutputStruct'];
        debugLog(`set ${class_name}, is_output_struct=${is_output_struct}`)
      }
      // Class name is set by the caller.
      result = new JsonMappingData(objName, class_name, '', '', is_output_struct);
      result.is_object = true;
      // Stored in temporary map to maintain sort order.
      const tmp_map = {};
      for (const key in jsonObj) {
        if (key != ':class') {
          if (key.lastIndexOf(':type') >= 0) {
            const key_name = key.split(':')[0];
            if (tmp_map[key_name]) {
              tmp_map[key_name].setType(jsonObj[key]);
            } else {
              const data = new JsonMappingData(key_name, jsonObj[key], '', class_name, is_output_struct);
              tmp_map[key_name] = data;
              debugLog(`set JsonMappingData = ${key_name}`)
            }
          }
        }
      }

      for (const key in jsonObj) {
        if (key.indexOf(':') == -1) {
          debugLog(`read key = ${key}`)
          const value = jsonObj[key];
          if (tmp_map[key]) {
            result.child_list[key] = tmp_map[key];
            result.child_list[key].init_value = value;
          } else {
            // type check
            let type_str = '';
            if (typeof value == 'string') {
              type_str = 'std::string';
            } else if (typeof value == 'number') {
              type_str = 'int64_t';
            } else if (typeof value == 'boolean') {
              type_str = 'bool';
            } else if (value) {
              const obj_key2 = Object.keys(value);
              if (obj_key2 == 0) { // array
                // Should I examine the element first?
                type_str = '';
              } else { // object
                type_str = '';
              }
            }
            result.child_list[key] = new JsonMappingData(key, type_str, value, class_name);
          }
          const temp_child = analyzeJson(value, key);
          if (result.child_list[key].type == '') {
            result.child_list[key].setTypeStruct(temp_child.type, temp_child.struct_type);
            if ((result.child_list[key].type.indexOf('JsonObjectVector') >= 0) ||
              (result.child_list[key].type.indexOf('JsonValueVector') >= 0)) {
              result.child_list[key].is_array = true;
            } else {
              result.child_list[key].is_object = true;
            }
          }
          result.child_list[key].child_list = temp_child.child_list;
          result.child_list[key].class_name = class_name;
        }
      }
    }
  } else {
    console.log('empty value.');
  }
  return result;
};

// ----------------------------------------------------------------------------
// analize child class function
// ----------------------------------------------------------------------------
const getChildClasses = (jsonMapData, list) => {
  if (!jsonMapData) {
    // do nothing
  } else if (jsonMapData.is_object) {
    for (const key in jsonMapData.child_list) {
      if (jsonMapData.child_list[key]) {
        if (jsonMapData.child_list[key].is_object || jsonMapData.child_list[key].is_array) {
          getChildClasses(jsonMapData.child_list[key], list);
        }
      }
    }
    list.push(jsonMapData);
  } else if (jsonMapData.is_array) {
    getChildClasses(jsonMapData.child_list[0], list);
  }
};

// ----------------------------------------------------------------------------
// generate cpp file source function
// ----------------------------------------------------------------------------
const generateFileSource = (filename, headerName, class_list, json_setting) => {
  const result = [];
  const namespace = json_setting.namespace;
  const include_nolint = (headerName.indexOf('/') >= 0) ? '' : '  // NOLINT';

  // header
  const source_file_header = `// Copyright 2019 CryptoGarage
/**
 * @file ${filename}
 *
 * @brief JSON mapping file (auto generate)
 */
#include <set>
#include <string>
#include <vector>

#include "${headerName}"${include_nolint}
`;
  result.push(source_file_header);

  if (isArray(namespace)) {
    for (let idx = 0; idx < namespace.length; ++idx) {
      result.push(`namespace ${namespace[idx]} {`);
    }
  } else {
    result.push(`namespace ${namespace} {`);
  }

  const source_file_header2 = `
using cfd::core::JsonClassBase;
using cfd::core::JsonObjectVector;
using cfd::core::JsonValueVector;
using cfd::core::JsonVector;
// clang-format off
// @formatter:off\
`;
  const source_file_footer = `
// @formatter:on
// clang-format on
`;
  result.push(source_file_header2);

  if (class_list) {
    for (const data in class_list) {
      result.push(class_list[data]);
    }
  }

  result.push(source_file_footer);

  if (isArray(namespace)) {
    for (let idx = namespace.length - 1; idx >= 0; --idx) {
      result.push(`}  // namespace ${namespace[idx]}`);
    }
  } else {
    result.push(`}  // namespace ${namespace}`);
  }
  result.push('');

  return result.join('\n');
};


// ----------------------------------------------------------------------------
// generate cpp class source function
// ----------------------------------------------------------------------------
const generateClassSource = (req, res) => {
  const result = [];
  const processed_list = [];

  if (req || res) {
    const list = [req, res];
    for (const data in list) {
      if (!data) continue;
      // sort by classname
      // for child elements
      const map_list = [];
      getChildClasses(list[data], map_list);
      debugLog(`map_list = ${map_list}`)

      for (const map_key in map_list) {
        const map_data = map_list[map_key];
        if (processed_list.includes(map_data.type)) {
          continue;
        }
        const source_class_header = `
// ------------------------------------------------------------------------
// ${map_data.type}
// ------------------------------------------------------------------------
cfd::core::JsonTableMap<${map_data.type}>
  ${map_data.type}::json_mapper;
std::vector<std::string> ${map_data.type}::item_list;

void ${map_data.type}::CollectFieldName() {
  if (!json_mapper.empty()) {
    return;
  }
  cfd::core::CLASS_FUNCTION_TABLE<${map_data.type}> func_table;  // NOLINT
`;
        result.push(source_class_header);

        for (const child_key in map_data.child_list) {
          const child_data = map_data.child_list[child_key];
          // start
          const add_json_mapper_comment = `\
  func_table = {
    ${map_data.type}::Get${child_data.method_name}String,
    ${map_data.type}::Set${child_data.method_name}String,
    ${map_data.type}::Get${child_data.method_name}FieldType,
  };
  json_mapper.emplace("${child_data.name}", func_table);
  item_list.push_back("${child_data.name}");\
`;
          // end
          result.push(add_json_mapper_comment);
        }
        result.push('}');

        if (map_data.is_output_struct) {
          result.push('');
          result.push(`void ${map_data.type}::ConvertFromStruct(`);
          result.push(`    const ${map_data.struct_type}& data) {`);
          for (const child_key in map_data.child_list) {
            const child_data = map_data.child_list[child_key];
            if (child_data.is_object || child_data.is_array) {
              result.push(`  ${child_data.variable_name}_.ConvertFromStruct(data.${child_data.variable_name});`);
            } else {
              result.push(`  ${child_data.variable_name}_ = data.${child_data.variable_name};`);
            }
          }
          result.push(`  ignore_items = data.ignore_items;`);
          result.push('}');

          result.push('');
          result.push(`${map_data.struct_type} ${map_data.type}::ConvertToStruct() const {  // NOLINT`);
          result.push(`  ${map_data.struct_type} result;`);
          for (const child_key in map_data.child_list) {
            const child_data = map_data.child_list[child_key];
            if (child_data.is_object || child_data.is_array) {
              result.push(`  result.${child_data.variable_name} = ${child_data.variable_name}_.ConvertToStruct();`);
            } else {
              result.push(`  result.${child_data.variable_name} = ${child_data.variable_name}_;`);
            }
          }
          result.push(`  result.ignore_items = ignore_items;`);
          result.push('  return result;');
          result.push('}');
        }

        processed_list.push(map_data.type);
      }
    }
  }

  return result.join('\n');
};


// ----------------------------------------------------------------------------
// generate class header function
// ----------------------------------------------------------------------------
const generateClassHeaderData = (map_data, export_define) => {
  const class_header = `
// ------------------------------------------------------------------------
// ${map_data.type}
// ------------------------------------------------------------------------
/**
 * @brief JSON-API (${map_data.type}) class
 */
class ${export_define}${map_data.type}
  : public cfd::core::JsonClassBase<${map_data.type}> {
 public:
  ${map_data.type}() {
    CollectFieldName();
  }
  virtual ~${map_data.type}() {
    // do nothing
  }
  /**
   * @brief collect field name.
   */
  static void CollectFieldName();
`;
  return class_header;
};


// ----------------------------------------------------------------------------
// generate object function by header
// ----------------------------------------------------------------------------
const generateObjectFunctionByHeader = (map_data, child_data) => {
  const object_functions = `\
  /**
   * @brief Get of ${child_data.name}.
   * @return ${child_data.name}
   */
  ${child_data.type}& Get${child_data.method_name}() {  // NOLINT
    return ${child_data.variable_name}_;
  }
  /**
   * @brief Set to ${child_data.name}.
   * @param[in] ${child_data.variable_name}    setting value.
   */
  void Set${child_data.method_name}(  // line separate
      const ${child_data.type}& ${child_data.variable_name}) {  // NOLINT
    this->${child_data.variable_name}_ = ${child_data.variable_name};
  }
  /**
   * @brief Get data type of ${child_data.name}.
   * @return Data type of ${child_data.name}.
   */
  static std::string Get${child_data.method_name}FieldType() {
    return "${child_data.type}";  // NOLINT
  }
  /**
   * @brief Get json string of ${child_data.name} field.
   * @param[in,out] obj     class object
   * @return JSON string.
   */
  static std::string Get${child_data.method_name}String(  // line separate
      const ${map_data.type}& obj) {  // NOLINT
    // Do not set to const, because substitution of member variables
    // may occur in pre / post processing inside Serialize
    return obj.${child_data.variable_name}_.Serialize();
  }
  /**
   * @brief Set json object to ${child_data.name} field.
   * @param[in,out] obj     class object
   * @param[in] json_value  JSON object
   */
  static void Set${child_data.method_name}String(  // line separate
      ${map_data.type}& obj,  // NOLINT
      const UniValue& json_value) {
    obj.${child_data.variable_name}_.DeserializeUniValue(json_value);
  }
`;
  return object_functions;
};

// ----------------------------------------------------------------------------
// generate value function by header
// ----------------------------------------------------------------------------
const generateValueFunctionByHeader = (map_data, child_data) => {
  const value_functions = `\
  /**
   * @brief Get of ${child_data.name}
   * @return ${child_data.name}
   */
  ${child_data.type} Get${child_data.method_name}() const {
    return ${child_data.variable_name}_;
  }
  /**
   * @brief Set to ${child_data.name}
   * @param[in] ${child_data.variable_name}    setting value.
   */
  void Set${child_data.method_name}(  // line separate
    const ${child_data.type}& ${child_data.variable_name}) {  // NOLINT
    this->${child_data.variable_name}_ = ${child_data.variable_name};
  }
  /**
   * @brief Get data type of ${child_data.name}
   * @return Data type of ${child_data.name}
   */
  static std::string Get${child_data.method_name}FieldType() {
    return "${child_data.type}";
  }
  /**
   * @brief Get json string of ${child_data.name} field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string Get${child_data.method_name}String(  // line separate
      const ${map_data.type}& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.${child_data.variable_name}_);
  }
  /**
   * @brief Set json object to ${child_data.name} field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void Set${child_data.method_name}String(  // line separate
      ${map_data.type}& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.${child_data.variable_name}_, json_value);
  }
`;
  return value_functions;
};

// ----------------------------------------------------------------------------
// generate class field by header
// ----------------------------------------------------------------------------
const generateClassFieldByHeader = (map_data) => {
  let struct_convert_function = '';
  if (map_data.is_output_struct) {
    struct_convert_function = `\
  /**
   * @brief Convert struct to class.
   * @param[in] data   struct data.
   */
  void ConvertFromStruct(
      const ${map_data.struct_type}& data);

  /**
   * @brief Convert class to struct.
   * @return  struct data.
   */
  ${map_data.struct_type} ConvertToStruct()  const;`;
  }

  const common_fields = `\
  /**
   * @brief Set ignore item.
   * @param[in] key   ignore target key name.
   */
  void SetIgnoreItem(const std::string& key) {
    ignore_items.insert(key);
  }

${struct_convert_function}

 protected:
  /**
   * @brief definition type of Map table.
   */
  using ${map_data.type}MapTable =
    cfd::core::JsonTableMap<${map_data.type}>;

  /**
   * @brief Get JSON mapping object.
   * @return JSON mapping object.
   * @see cfd::core::JsonClassBase::GetJsonMapper()
   */
  virtual const ${map_data.type}MapTable& GetJsonMapper() const {  // NOLINT
    return json_mapper;
  }
  /**
   * @brief Get item lists of JSON mapping.
   * Fetch a list of target variable names in the order of definition.
   * @return Item lists of JSON mapping.
   * @see cfd::core::JsonClassBase::GetJsonItemList()
   */
  virtual const std::vector<std::string>& GetJsonItemList() const {
    return item_list;
  }
  /**
   * @brief Get ignore item lists of JSON mnapping.
   * Ignore the target variable at Serialize.
   * @return Item list of JSON mapping.
   * @see cfd::core::JsonClassBase::GetIgnoreItem()
   */
  virtual const std::set<std::string>& GetIgnoreItem() const {
    return ignore_items;
  }

 private:
 /**
  * @brief JsonFunctionMap table
  */
  static ${map_data.type}MapTable json_mapper;
  /**
   * @brief field name list.
   */
  static std::vector<std::string> item_list;
  /**
   * @brief ignore item list.
   */
  std::set<std::string> ignore_items;
`;
  return common_fields;
};

// ----------------------------------------------------------------------------
// generate header function
// ----------------------------------------------------------------------------
const generateFileHeader = (filename, dirname, class_list, json_setting, append_header_name = '') => {
  const result = [];

  const namespace = json_setting.namespace;
  let path = `${dirname}/${filename}_`;
  if (path.startsWith(__dirname)) {
    path = path.substr(__dirname.length);
  }
  while (path.indexOf('/', 0) == 0) {
    path = path.substr(1);
  }
  while (path.indexOf('../') >= 0) {
    path = path.replace('../', '');
  }
  while (path.indexOf('./') >= 0) {
    path = path.replace('./', '');
  }
  while (path.indexOf('//') >= 0) {
    path = path.replace('//', '/');
  }
  while (path.indexOf('external/') >= 0) {
    path = path.replace('external/', '');
  }
  while (path.indexOf('/') >= 0) {
    path = path.replace('/', '_');
  }
  while (path.indexOf('.') >= 0) {
    path = path.replace('.', '_');
  }
  while (path.indexOf('-') >= 0) {
    path = path.replace('-', '_');
  }
  const def_name = path.toUpperCase();
  const include_header = (json_setting.common_header) ? `#include "${json_setting.common_header}"\n` : '';
  const include_header2 = (append_header_name.length > 0) ? `#include "${append_header_name}"\n` : '';

  // header
  const header_file_header = `// Copyright 2019 CryptoGarage
/**
 * @file ${filename}
 *
 * @brief JSON mapping file. (auto generate)
 */
#ifndef ${def_name}
#define ${def_name}

#include <set>
#include <string>
#include <vector>

#include "cfdcore/cfdcore_json_mapping_base.h"
${include_header}
${include_header2}`;

  result.push(header_file_header);

  if (isArray(namespace)) {
    for (let idx = 0; idx < namespace.length; ++idx) {
      result.push(`namespace ${namespace[idx]} {`);
    }
  } else {
    result.push(`namespace ${namespace} {`);
  }

  const header_file_header2 = `
using cfd::core::JsonClassBase;
using cfd::core::JsonObjectVector;
using cfd::core::JsonValueVector;
using cfd::core::JsonVector;
// clang-format off
// @formatter:off\
`;

  const header_file_footer = `
// @formatter:on
// clang-format on
`;
  const header_file_footer2 = `
#endif  // ${def_name}
`;
  result.push(header_file_header2);

  if (class_list) {
    for (const data in class_list) {
      result.push(class_list[data]);
    }
  }

  result.push(header_file_footer);
  if (isArray(namespace)) {
    for (let idx = namespace.length - 1; idx >= 0; --idx) {
      result.push(`}  // namespace ${namespace[idx]}`);
    }
  } else {
    result.push(`}  // namespace ${namespace}`);
  }
  result.push(header_file_footer2);
  return result.join('\n');
};

// ----------------------------------------------------------------------------
// generate class header function
// ----------------------------------------------------------------------------
const generateClassHeader = (req, res, json_setting) => {
  const result = [];
  const processed_list = [];
  const export_define = (json_setting.export) ? `${json_setting.export} ` : '';

  // header
  if (req || res) {
    const list = [req, res];
    for (const data in list) {
      if (!data) continue;
      // sort by generate class
      // Child element in order
      const map_list = [];
      getChildClasses(list[data], map_list);
      debugLog(`map_list = ${map_list}`)

      for (const map_key in map_list) {
        const map_data = map_list[map_key];
        if (processed_list.includes(map_data.type)) {
          continue;
        }
        const class_header = generateClassHeaderData(map_data, export_define);
        result.push(class_header);

        for (const child_key in map_data.child_list) {
          const child_data = map_data.child_list[child_key];
          if (child_data.is_object || child_data.is_array) {
            const object_functions = generateObjectFunctionByHeader(map_data, child_data);
            result.push(`${object_functions}`);
          } else {
            const value_functions = generateValueFunctionByHeader(map_data, child_data);
            result.push(value_functions);
          }
        }
        const common_fields = generateClassFieldByHeader(map_data);
        result.push(common_fields);

        for (const child_key in map_data.child_list) {
          const child_data = map_data.child_list[child_key];
          const object_fields = `\
  /**
   * @brief JsonAPI(${child_data.name}) value
   */`;
          result.push(object_fields);
          if (child_data.is_object || child_data.is_array) {
            result.push(`  ${child_data.type} ${child_data.variable_name}_;  // NOLINT`);
          } else if (child_data.type == 'std::string') {
            // string
            result.push(`  ${child_data.type} ${child_data.variable_name}_ = "${child_data.init_value}";`);
          } else {
            result.push(`  ${child_data.type} ${child_data.variable_name}_ = ${child_data.init_value};`);
          }
        }

        result.push('};');

        processed_list.push(map_data.type);
      }
    }
  }

  return result.join('\n');
};


// ----------------------------------------------------------------------------
// generate struct header function
// ----------------------------------------------------------------------------
const generateStructHeaderArea = (map_data) => {
  const struct_header = `
// ------------------------------------------------------------------------
// ${map_data.struct_type}
// ------------------------------------------------------------------------
/**
 * @brief ${map_data.struct_type} struct
 */
struct ${map_data.struct_type} {`;
  return struct_header;
};

// ----------------------------------------------------------------------------
// generate struct item data function
// ----------------------------------------------------------------------------
const generateStructItemData = (text_array, req, res, json_data, last_namespaces, has_error_output, processed_list) => {
  if (req || res) {
    let namespace = '';
    let last_namespace = '';
    if (isArray(json_data.namespace)) {
      for (let idx = 0; idx < json_data.namespace.length; ++idx) {
        if (namespace.length > 0) {
          namespace += '::' + json_data.namespace[idx];
        } else {
          namespace = json_data.namespace[idx];
        }
      }
    } else {
      namespace = json_data.namespace;
    }
    if (isArray(last_namespaces)) {
      for (let idx = 0; idx < last_namespaces.length; ++idx) {
        if (last_namespace.length > 0) {
          last_namespace += '::' + last_namespaces[idx];
        } else {
          last_namespace = last_namespaces[idx];
        }
      }
    } else {
      last_namespace = last_namespaces;
    }

    if (namespace != last_namespace) {
      if (last_namespace.length > 0) {
        text_array.push('');
        if (isArray(last_namespaces)) {
          for (let idx = last_namespaces.length - 1; idx >= 0; --idx) {
            if (last_namespaces[idx] != 'json') {
              text_array.push(`}  // namespace ${last_namespaces[idx]}`);
            }
          }
        } else {
          text_array.push(`}  // namespace ${last_namespace}`);
        }
        text_array.push('');
      }

      if (isArray(json_data.namespace)) {
        for (let idx = 0; idx < json_data.namespace.length; ++idx) {
          if (json_data.namespace[idx] != 'json') {
            text_array.push(`namespace ${json_data.namespace[idx]} {`);
          }
        }
      } else {
        text_array.push(`namespace ${namespace} {`);
      }
    }

    const list = [req, res];
    for (const data in list) {
      if (!data) continue;
      const map_list = [];
      getChildClasses(list[data], map_list);

      for (const map_key in map_list) {
        const map_data = map_list[map_key];
        if (!map_data.is_output_struct || processed_list.includes(map_data.struct_type)) {
          console.log(`skip output struct: ${map_data.struct_type}`);
          continue;
        }
        const struct_header = generateStructHeaderArea(map_data);
        text_array.push(struct_header);

        let name_length_max = 0;
        for (const child_key in map_data.child_list) {
          const child_data = map_data.child_list[child_key];
          let name_length = child_data.type.length + child_data.variable_name.length;
          if (child_data.is_object || child_data.is_array) {
            name_length = child_data.struct_type.length + child_data.variable_name.length;
          } else if (child_data.type == 'std::string') {
            name_length += child_data.init_value.length + 5;
          } else {
            const string_text = `${child_data.init_value}`;
            name_length += string_text.length + 3;
          }
          if (name_length_max < name_length) name_length_max = name_length;
        }

        for (const child_key in map_data.child_list) {
          const child_data = map_data.child_list[child_key];
          let name_length = child_data.type.length + child_data.variable_name.length;
          if (child_data.is_object || child_data.is_array) {
            name_length = child_data.struct_type.length + child_data.variable_name.length;
          } else if (child_data.type == 'std::string') {
            name_length += child_data.init_value.length + 5;
          } else {
            const string_text = `${child_data.init_value}`;
            name_length += string_text.length + 3;
          }
          let space = '';
          if (name_length_max > name_length) {
            name_length = name_length_max - name_length;
            for (let i = 0; i < name_length; ++i) space += ' ';
          }
          const comment = `${space}//!< ${child_data.variable_name}  // NOLINT`;

          if (child_data.is_object || child_data.is_array) {
            text_array.push(`  ${child_data.struct_type} ${child_data.variable_name};  ${comment}`);
          } else if (child_data.type == 'std::string') {
            // string
            text_array.push(`  ${child_data.type} ${child_data.variable_name} = "${child_data.init_value}";  ${comment}`);
          } else {
            text_array.push(`  ${child_data.type} ${child_data.variable_name} = ${child_data.init_value};  ${comment}`);
          }
        }

        /*
        if (has_error_output && (map_data.struct_type.indexOf('ResponseStruct') >= 0)) {
          if (namespace == 'cfd::js::api') {
            text_array.push(`  InnerErrorResponseStruct error;       //!< error information`);
          } else {
            text_array.push(`  cfd::js::api::InnerErrorResponseStruct error;   //!< error information`);
          }
        }
        */
        text_array.push(`  std::set<std::string> ignore_items;   //!< using on JSON mapping convert.`);
        text_array.push('};');

        processed_list.push(map_data.struct_type);
      }
    }
  }
};

// ----------------------------------------------------------------------------
// generate struct header function
// ----------------------------------------------------------------------------
const generateStructHeader = (dirname, filename, json_list) => {
  const result = [];
  const processedStructTypes = [];

  let path = `${dirname}/${filename}_`;
  if (path.startsWith(__dirname)) {
    path = path.substr(__dirname.length);
  }
  while (path.indexOf('/', 0) == 0) {
    path = path.substr(1);
  }
  while (path.indexOf('../') >= 0) {
    path = path.replace('../', '');
  }
  while (path.indexOf('./') >= 0) {
    path = path.replace('./', '');
  }
  while (path.indexOf('//') >= 0) {
    path = path.replace('//', '/');
  }
  while (path.indexOf('external/') >= 0) {
    path = path.replace('external/', '');
  }
  while (path.indexOf('/') >= 0) {
    path = path.replace('/', '_');
  }
  while (path.indexOf('.') >= 0) {
    path = path.replace('.', '_');
  }
  while (path.indexOf('-') >= 0) {
    path = path.replace('-', '_');
  }
  const def_name = path.toUpperCase();

  // header
  const header_file_header = `// Copyright 2019 CryptoGarage
/**
 * @file ${filename}
 *
 * @brief Struct mapping file. (auto generate)
 */
#ifndef ${def_name}
#define ${def_name}

#include <cstdint>
#include <set>
#include <string>
#include <vector>

// clang-format off
// @formatter:off\
`;
  const header_file_footer = `
// @formatter:on
// clang-format on

#endif  // ${def_name}
`;
  result.push(header_file_header);

  let last_namespace = '';
  for (const json_data_index in json_list) {
    if (('priority' in json_list[json_data_index].json_data) &&
      (json_list[json_data_index].json_data['priority'] == 'high')) {
      const req = json_list[json_data_index].request_data;
      const res = json_list[json_data_index].response_data;
      generateStructItemData(result, req, res,
          json_list[json_data_index].json_data, last_namespace, false,
          processedStructTypes);
      last_namespace = json_list[json_data_index].json_data.namespace;
    }
  }

  for (const json_data_index in json_list) {
    if (('priority' in json_list[json_data_index].json_data) &&
      (json_list[json_data_index].json_data['priority'] == 'high')) {
      // do nothing
    } else {
      const req = json_list[json_data_index].request_data;
      const res = json_list[json_data_index].response_data;
      generateStructItemData(result, req, res,
          json_list[json_data_index].json_data, last_namespace, true,
          processedStructTypes);
      const json_data = json_list[json_data_index].json_data;
      last_namespace = json_list[json_data_index].json_data.namespace;
    }
  }
  result.push('');
  if (isArray(last_namespace)) {
    for (let idx = last_namespace.length - 1; idx >= 0; --idx) {
      if (last_namespace[idx] != 'json') {
        result.push(`}  // namespace ${last_namespace[idx]}`);
      }
    }
  } else {
    result.push(`}  // namespace ${last_namespace}`);
  }

  result.push(header_file_footer);
  return result.join('\n');
};

// ----------------------------------------------------------------------------
// search file
// ----------------------------------------------------------------------------
function convertFile() {
  const fileList = [];
  let cfdBaseDir;
  const cfdPath = `${__dirname}/../external/cfd/`;
  const cfdPath2 = `${__dirname}/../../cfd/`;
  let folderPath = `src/jsonapi/input_json_format/`;
  const outJsonSourceFolderPath = `${__dirname}/../../cfd/src/jsonapi/autogen/`;
  const outJsonHeaderFolderPath = `${__dirname}/../../cfd/src/jsonapi/autogen/`;
  let outStructDirPath = `src/jsonapi/`;
  let outTsFolderPath = ``;
  const outStructFileName = `cfd_struct.h`;
  // const outTsFileName = `index.d.ts`;
  let classHeaderList = [];
  let classSourceList = [];
  let jsonDataList = [];
  let jsonClassMap = {};
  let jsonTypeList = [];
  let functionList = [];

  if (fs.existsSync(cfdPath) && fs.statSync(cfdPath).isDirectory()) {
    cfdBaseDir = cfdPath;
    outTsFolderPath = cfdPath + outTsFolderPath;
    folderPath = cfdPath + folderPath;
    outStructDirPath = cfdPath + outStructDirPath;
  } else {
    cfdBaseDir = cfdPath2;
    outTsFolderPath = `${__dirname}/`;  // relative path from tsconfig.json
    folderPath = cfdPath2 + folderPath;
    outStructDirPath = cfdPath2 + outStructDirPath;
  }

  let jsonObjectCommon = undefined;
  fs.readdir(folderPath, (err, files) => {
    if (err) throw err;
    files.filter(function(file) {
      // filter
      return fs.statSync(`${folderPath}${file}`).isFile() && /.*\.json$/.test(file);
    }).forEach(function(file) {
      console.log(`file = ${file}`);
      const inFile = `${folderPath}${file}`;
      const outFile = file.replace(/\.json$/i, '_json');
      const outHeaderFile = `${outFile}.h`;
      const outSourceFile = `${outFile}.cpp`;

      // read json
      const jsonObject = JSON.parse(fs.readFileSync(inFile, 'utf8'));
      const reqData = (jsonObject.request) ? analyzeJson(jsonObject.request, 'root') : null;
      const resData = (jsonObject.response) ? analyzeJson(jsonObject.response, 'root') : null;
      let funcName = '';
      if (reqData != null) {
        const ret = reqData.collectMapData(jsonClassMap, jsonTypeList);
        jsonClassMap = ret['map'];
        jsonTypeList = ret['list'];
        funcName = reqData.getFunctionName();
      }
      if (resData != null) {
        const ret = resData.collectMapData(jsonClassMap, jsonTypeList);
        jsonClassMap = ret['map'];
        jsonTypeList = ret['list'];
        if (funcName === '') funcName = resData.getFunctionName();
      }
      if (funcName !== '') functionList.push(funcName);
      debugLog(`reqData = ${reqData}`)
      debugLog(`resData = ${resData}`)
      jsonDataList.push(new JsonData(jsonObject, reqData, resData));

      const header_str = generateClassHeader(reqData, resData, jsonObject);
      classHeaderList.push(header_str);
      const src_str = generateClassSource(reqData, resData);
      classSourceList.push(src_str);
      if (jsonObjectCommon === undefined) {
        jsonObjectCommon = jsonObject;
      } else {
        if (jsonObject.namespace && jsonObject.namespace.length > 0) {
          jsonObjectCommon['namespace'] = jsonObject.namespace;
        }
        if (jsonObject.common_header && jsonObject.common_header.length > 0) {
          jsonObjectCommon['common_header'] = jsonObject.common_header;
        }
      }
    });

    {
      let namespace_name = '';
      const namespace = jsonObjectCommon.namespace;
      if (isArray(namespace)) {
        for (let idx = 0; idx < namespace.length; ++idx) {
          if (idx !== 0) namespace_name += '_';
          namespace_name += namespace[idx];
        }
      } else {
        namespace_name += namespace;
      }
    
      const outHeaderFile = `${namespace_name}_autogen.h`;
      const outSourceFile = `${namespace_name}_autogen.cpp`;
      const header_str = generateFileHeader(outHeaderFile, outJsonHeaderFolderPath,
          classHeaderList, jsonObjectCommon, `jsonapi/${outStructFileName}`);
      fs.writeFileSync(`${outJsonHeaderFolderPath}${outHeaderFile}`, header_str);
      const src_str = generateFileSource(outSourceFile, outHeaderFile,
          classSourceList, jsonObjectCommon);
      fs.writeFileSync(`${outJsonSourceFolderPath}${outSourceFile}`, src_str);
    };

    if (jsonDataList.length > 0) {
      const header_str = generateStructHeader(outStructDirPath, outStructFileName, jsonDataList);
      fs.writeFileSync(`${outStructDirPath}${outStructFileName}`, header_str);
      console.log(`output: ${outStructFileName}`);
    }
  });
};


const main = function() {
  for (let i = 2; i < process.argv.length; i++){
    if (process.argv[i]) {
      if (process.argv[i] === 'mode=debug') {
        debugLog = function() {
          switch (arguments.length) {
            case 1:
              console.log(arguments[0]);
              break;
            case 2:
              console.log(arguments[0], arguments[1]);
              break;
            case 3:
              console.log(arguments[0], arguments[1], arguments[2]);
              break;
            default:
              console.log(Array.prototype.join.call(arguments));
              break;
          }
        }
      }
    }
  }

  convertFile();
};
main();