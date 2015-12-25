// Copyright 2015, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/json
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#pragma once

#include <string>
#include <sstream>
#include <unordered_map>

#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>

#include "../json/range.h"

using google::protobuf::Message;
using google::protobuf::Descriptor;
using google::protobuf::FieldDescriptor;
using google::protobuf::Reflection;
using google::protobuf::EnumValueDescriptor;
using rapidjson::Document;
using rapidjson::Value;
using rapidjson::kObjectType;
using rapidjson::kArrayType;
using rapidjson::StringBuffer;
using rapidjson::Writer;
using AllocatorType = typename Document::AllocatorType;
using json::range;

class Format {
 public:
  static std::string Pb2Json(const Message& pb) {
    Document json;
    Pb2Json(pb, json);
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    json.Accept(writer);
    return buffer.GetString();
  }

  static Document& Pb2Json(const Message& pb, Document& json) {
    json.SetObject();
    Pb2Json(pb, json, json.GetAllocator());
    return json;
  }

  static bool Json2Pb(const std::string& json, Message& pb) {
    Document doc;
    if (doc.Parse<0>(json.c_str()).HasParseError()) {
      return false;
    }
    return Value2Pb(doc, pb);
  }

 private:
  static void Pb2Json(const Message& pb, Value& json, AllocatorType& allocator) {
    const Reflection* refl = pb.GetReflection();
    std::vector<const FieldDescriptor*> fields;
    // repeated fields will only be listed if FieldSize(field) would return non-zero.
    refl->ListFields(pb, &fields);
    for (int i : range(fields.size())) {
      fields[i]->is_repeated() ? AddRepeatedJson(pb, fields[i], refl, json, allocator) :
          AddNonrepeatedJson(pb, fields[i], refl, json, allocator);
    }
  }

  static void AddRepeatedJson(const Message& pb,
      const FieldDescriptor* field, const Reflection* refl,
      Value& json, AllocatorType& allocator) {
    Value value_array(kArrayType);
    switch (field->cpp_type()) {
#define SerializeRepeatedToJsonObject(cpp_type, method)                     \
    case FieldDescriptor::CPPTYPE_##cpp_type:                               \
      for (int i : range(refl->FieldSize(pb, field))) {                     \
        value_array.PushBack(refl->GetRepeated##method(pb, field, i), allocator); \
      }                                                                     \
      break
    SerializeRepeatedToJsonObject(INT32 , Int32 );
    SerializeRepeatedToJsonObject(INT64 , Int64 );
    SerializeRepeatedToJsonObject(UINT32, UInt32);
    SerializeRepeatedToJsonObject(UINT64, UInt64);
    SerializeRepeatedToJsonObject(DOUBLE, Double);
    SerializeRepeatedToJsonObject(FLOAT , Float );
    SerializeRepeatedToJsonObject(BOOL  , Bool  );
#undef SerializeRepeatedToJsonObject
    case FieldDescriptor::CPPTYPE_STRING:
      for(int i : range(refl->FieldSize(pb, field))) {
        Value value_string;
        value_string.SetString(refl->GetRepeatedString(pb, field, i).c_str(), allocator);
        value_array.PushBack(value_string, allocator);
      }
      break;
    case FieldDescriptor::CPPTYPE_ENUM:
      for(int i : range(refl->FieldSize(pb, field))) {
        value_array.PushBack(refl->GetRepeatedEnum(pb, field, i)->number(), allocator);
      }
      break;
    case FieldDescriptor::CPPTYPE_MESSAGE:
      for (int i : range(refl->FieldSize(pb, field))) {
        Value value(kObjectType);
        Pb2Json(refl->GetRepeatedMessage(pb, field, i), value, allocator);
        value_array.PushBack(value, allocator);
      }
      break;
    default:
      break;
    }
    json.AddMember(field->name().c_str(), value_array, allocator);
  }

  static inline void AddNonrepeatedJson(const Message& pb,
      const FieldDescriptor* field, const Reflection* refl,
      Value& json, AllocatorType& allocator) {
    if (!refl->HasField(pb, field)) {
      // Done't output null field.
      return;
    }
    switch (field->cpp_type()) {
#define SerializeNonrepeatedToJsonObject(cpp_type, method)                  \
    case FieldDescriptor::CPPTYPE_##cpp_type:                               \
      json.AddMember(field->name().c_str(), refl->Get##method(pb, field), allocator);\
      break
    SerializeNonrepeatedToJsonObject(INT32 , Int32 );
    SerializeNonrepeatedToJsonObject(INT64 , Int64 );
    SerializeNonrepeatedToJsonObject(UINT32, UInt32);
    SerializeNonrepeatedToJsonObject(UINT64, UInt64);
    SerializeNonrepeatedToJsonObject(DOUBLE, Double);
    SerializeNonrepeatedToJsonObject(FLOAT , Float );
    SerializeNonrepeatedToJsonObject(BOOL  , Bool  );
#undef SerializeNonrepeatedToJsonObject
    case FieldDescriptor::CPPTYPE_STRING:
      {
        Value value_string;
        value_string.SetString(refl->GetString(pb, field).c_str(), allocator);
        json.AddMember(field->name().c_str(), value_string, allocator);
      }
      break;
    case FieldDescriptor::CPPTYPE_ENUM:
      json.AddMember(field->name().c_str(), refl->GetEnum(pb, field)->number(), allocator);
      break;
    case FieldDescriptor::CPPTYPE_MESSAGE:
      {
        Value value(kObjectType);
        Pb2Json(refl->GetMessage(pb, field), value, allocator);
        json.AddMember(field->name().c_str(), value, allocator);
      }
      break;
    default:
      break;
    }
  }

  static bool Value2Pb(const Value& json, Message& pb) {
    const Descriptor* desc = pb.GetDescriptor();
    const Reflection* refl = pb.GetReflection();
    for (int i : range(desc->field_count())) {
      const FieldDescriptor* field = desc->field(i);
      const Value& value = json[field->name().c_str()];
      if (value.IsNull() && field->is_required()) {
        // Required key doesn't exist in json.
        return false;
      }
      if (field->is_repeated() && value.IsArray()) {
        for (Value::ConstValueIterator value_ite = value.Begin();
            value_ite != value.End(); ++value_ite) {
          if (!AddValue(*value_ite, field, refl, pb)) {
            return false;
          }
        }
        continue;
      }
      if (!SetValue(value, field, refl, pb)) {
        return false;
      }
    }
    return true;
  }

  static bool AddValue(const Value& value, const FieldDescriptor* field,
      const Reflection* refl, Message& pb) {
    switch (field->cpp_type()) {
#define AddValueByType(cpp_type, method, value_type)                        \
    case FieldDescriptor::CPPTYPE_##cpp_type:                               \
      if (value.IsNull()) {                                                 \
        return true;                                                        \
      }                                                                     \
      refl->Add##method(&pb, field, value.Get##value_type());               \
      break
    AddValueByType(INT32 , Int32 , Int   );
    AddValueByType(INT64 , Int64 , Int64 );
    AddValueByType(UINT32, UInt32, Uint  );
    AddValueByType(UINT64, UInt64, Uint64);
    AddValueByType(DOUBLE, Double, Double);
    AddValueByType(FLOAT , Float , Double); // FIXME
    AddValueByType(BOOL  , Bool  , Int   );
#undef AddValueByType
    case FieldDescriptor::CPPTYPE_STRING:
      if (value.IsNull()) {
        return true;
      }
      refl->AddString(&pb, field, value.GetString());
      break;
    case FieldDescriptor::CPPTYPE_ENUM:
      if (value.IsNull()) {
        return true;
      }
      {
        const EnumValueDescriptor* enum_desc =
            field->enum_type()->FindValueByNumber(value.GetInt());
        if (!enum_desc) {
          // Invalid enum value.
          return false;
        }
        refl->AddEnum(&pb, field, enum_desc);
      }
      break;
    case FieldDescriptor::CPPTYPE_MESSAGE:
      if (value.IsNull()) {
        return true;
      }
      return Value2Pb(value, *refl->AddMessage(&pb, field));
    default:
      break;
    }
    return true;
  }

  static bool SetValue(const Value& value, const FieldDescriptor* field,
      const Reflection* refl, Message& pb) {
    switch (field->cpp_type()) {
#define SetValueByType(cpp_type, method, value_type)                        \
    case FieldDescriptor::CPPTYPE_##cpp_type:                               \
      if (value.IsNull()) {                                                 \
        return true;                                                        \
      }                                                                     \
      refl->Set##method(&pb, field, value.Get##value_type());               \
      break
    SetValueByType(INT32 , Int32 , Int   );
    SetValueByType(INT64 , Int64 , Int64 );
    SetValueByType(UINT32, UInt32, Uint  );
    SetValueByType(UINT64, UInt64, Uint64);
    SetValueByType(DOUBLE, Double, Double);
    SetValueByType(FLOAT , Float , Double); // FIXME
    SetValueByType(BOOL  , Bool  , Int   );
#undef SetValueByType
    case FieldDescriptor::CPPTYPE_STRING:
      if (value.IsNull()) {
        return true;
      }
      refl->SetString(&pb, field, value.GetString());
      break;
    case FieldDescriptor::CPPTYPE_ENUM:
      if (value.IsNull()) {
        return true;
      }
      {
        const EnumValueDescriptor* enum_desc =
            field->enum_type()->FindValueByNumber(value.GetInt());
        if (!enum_desc) {
          // Invalid enum value.
          return false;
        }
        refl->SetEnum(&pb, field, enum_desc);
      }
      break;
    case FieldDescriptor::CPPTYPE_MESSAGE:
      if (value.IsNull()) {
        return true;
      }
      return Value2Pb(value, *refl->MutableMessage(&pb, field));
    default:
      break;
    }
    return true;
  }
};
