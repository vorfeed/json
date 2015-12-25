// Copyright 2015, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/json
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#pragma once

#ifndef JSON_FORMAT_H
#error "This should only be included by json_format.h"
#endif

namespace json {

std::string Format::Pb2Json(const Message& pb) {
  Json json;
  return Pb2Json(pb, json).str();
}

Json& Format::Pb2Json(const Message& pb, Json& json) {
  const Reflection* refl = pb.GetReflection();
  std::vector<const FieldDescriptor*> fields;
  // repeated fields will only be listed if FieldSize(field) would return non-zero.
  refl->ListFields(pb, &fields);
  for (int i : range(fields.size())) {
    fields[i]->is_repeated() ? AddRepeatedJson(pb, fields[i], refl, json) :
        AddNonrepeatedJson(pb, fields[i], refl, json);
  }
  return json;
}

bool Format::Json2Pb(const std::string& json, Message& pb) {
  Json object(Json::Create(json));
  if (object.Is<void>()) {
    return false;
  }
  return Json2Pb(object, pb);
}

bool Format::Json2Pb(const Json& json, Message& pb) {
  const Descriptor* desc = pb.GetDescriptor();
  const Reflection* refl = pb.GetReflection();
  for (int i : range(desc->field_count())) {
    const FieldDescriptor* field = desc->field(i);
    const auto ite = json.As<Mapping>().find(field->name());
    if (ite == json.As<Mapping>().end()) {
      if (field->is_required()) {
        return false;
      }
      continue;
    }
    const Json& value = ite->second;
    if (value.Is<void>() && field->is_required()) {
      // required key doesn't exist in json.
      return false;
    }
    if (field->is_repeated() && value.Is<Sequence>()) {
      for (auto value_ite = value.As<Sequence>().begin();
          value_ite != value.As<Sequence>().end(); ++value_ite) {
        if (!AddJson(*value_ite, field, refl, pb)) {
          return false;
        }
      }
      continue;
    }
    if (!SetJson(value, field, refl, pb)) {
      return false;
    }
  }
  return true;
}

void Format::AddRepeatedJson(const Message& pb,
    const FieldDescriptor* field, const Reflection* refl, Json& json) {
  Sequence sequence;
  switch (field->cpp_type()) {
#define SerializeRepeatedToJsonObject(cpp_type, method)                     \
  case FieldDescriptor::CPPTYPE_##cpp_type:                                 \
    sequence.reserve(refl->FieldSize(pb, field));                           \
    for (int i : range(refl->FieldSize(pb, field))) {                       \
      sequence.emplace_back(refl->GetRepeated##method(pb, field, i));       \
    }                                                                       \
    break
  SerializeRepeatedToJsonObject(INT32 , Int32 );
  SerializeRepeatedToJsonObject(INT64 , Int64 );
  SerializeRepeatedToJsonObject(UINT32, UInt32);
  SerializeRepeatedToJsonObject(UINT64, UInt64);
  SerializeRepeatedToJsonObject(DOUBLE, Double);
  SerializeRepeatedToJsonObject(FLOAT , Float );
  SerializeRepeatedToJsonObject(BOOL  , Bool  );
  SerializeRepeatedToJsonObject(STRING, String);
#undef SerializeRepeatedToJsonObject
  case FieldDescriptor::CPPTYPE_ENUM:
    sequence.reserve(refl->FieldSize(pb, field));
    for(int i : range(refl->FieldSize(pb, field))) {
      sequence.emplace_back(refl->GetRepeatedEnum(pb, field, i)->number());
    }
    break;
  case FieldDescriptor::CPPTYPE_MESSAGE:
    sequence.reserve(refl->FieldSize(pb, field));
    for (int i : range(refl->FieldSize(pb, field))) {
      Json value;
      Pb2Json(refl->GetRepeatedMessage(pb, field, i), value);
      sequence.emplace_back(value);
    }
    break;
  default:
    break;
  }
  json[field->name()] = std::move(sequence);
}

void Format::AddNonrepeatedJson(const Message& pb,
    const FieldDescriptor* field, const Reflection* refl, Json& json) {
  if (!refl->HasField(pb, field)) {
    // done't output null field.
    return;
  }
  switch (field->cpp_type()) {
#define SerializeNonrepeatedToJsonObject(cpp_type, method)                  \
  case FieldDescriptor::CPPTYPE_##cpp_type:                                 \
    json[field->name()] = refl->Get##method(pb, field);                     \
    break
  SerializeNonrepeatedToJsonObject(INT32 , Int32 );
  SerializeNonrepeatedToJsonObject(INT64 , Int64 );
  SerializeNonrepeatedToJsonObject(UINT32, UInt32);
  SerializeNonrepeatedToJsonObject(UINT64, UInt64);
  SerializeNonrepeatedToJsonObject(DOUBLE, Double);
  SerializeNonrepeatedToJsonObject(FLOAT , Float );
  SerializeNonrepeatedToJsonObject(BOOL  , Bool  );
  SerializeNonrepeatedToJsonObject(STRING, String);
#undef SerializeNonrepeatedToJsonObject
  case FieldDescriptor::CPPTYPE_ENUM:
    json[field->name()] = refl->GetEnum(pb, field)->number();
    break;
  case FieldDescriptor::CPPTYPE_MESSAGE:
    {
      Json value;
      Pb2Json(refl->GetMessage(pb, field), value);
      json[field->name()] = std::move(value);
    }
    break;
  default:
    break;
  }
}

bool Format::AddJson(const Json& value, const FieldDescriptor* field,
    const Reflection* refl, Message& pb) {
  switch (field->cpp_type()) {
#define AddJsonByType(cpp_type, method, value_type)                         \
  case FieldDescriptor::CPPTYPE_##cpp_type:                                 \
    if (value.Is<void>()) {                                                 \
      return true;                                                          \
    }                                                                       \
    if (!value.CanBe<value_type>()) {                                       \
      return false;                                                         \
    }                                                                       \
    refl->Add##method(&pb, field, value.As<value_type>());                  \
    break
  AddJsonByType(INT32 , Int32 , int32_t    );
  AddJsonByType(INT64 , Int64 , int64_t    );
  AddJsonByType(UINT32, UInt32, uint32_t   );
  AddJsonByType(UINT64, UInt64, uint64_t   );
  AddJsonByType(DOUBLE, Double, double     );
  AddJsonByType(FLOAT , Float , float      );
  AddJsonByType(BOOL  , Bool  , bool       );
  AddJsonByType(STRING, String, std::string);
#undef AddJsonByType
  case FieldDescriptor::CPPTYPE_ENUM:
    if (value.Is<void>()) {
      return true;
    }
    if (!value.CanBe<int>()) {
      return false;
    }
    {
      const EnumValueDescriptor* enum_desc =
          field->enum_type()->FindValueByNumber(value.As<int>());
      if (!enum_desc) {
        // invalid enum value.
        return false;
      }
      refl->AddEnum(&pb, field, enum_desc);
    }
    break;
  case FieldDescriptor::CPPTYPE_MESSAGE:
    if (value.Is<void>()) {
      return true;
    }
    if (!value.CanBe<Mapping>()) {
      return false;
    }
    return Json2Pb(value, *refl->AddMessage(&pb, field));
  default:
    break;
  }
  return true;
}

bool Format::SetJson(const Json& value, const FieldDescriptor* field,
    const Reflection* refl, Message& pb) {
  switch (field->cpp_type()) {
#define SetJsonByType(cpp_type, method, value_type)                         \
  case FieldDescriptor::CPPTYPE_##cpp_type:                                 \
    if (value.Is<void>()) {                                                 \
      return true;                                                          \
    }                                                                       \
    if (!value.CanBe<value_type>()) {                                       \
      return false;                                                         \
    }                                                                       \
    refl->Set##method(&pb, field, value.As<value_type>());                  \
    break
  SetJsonByType(INT32 , Int32 , int32_t    );
  SetJsonByType(INT64 , Int64 , int64_t    );
  SetJsonByType(UINT32, UInt32, uint32_t   );
  SetJsonByType(UINT64, UInt64, uint64_t   );
  SetJsonByType(DOUBLE, Double, double     );
  SetJsonByType(FLOAT , Float , float      );
  SetJsonByType(BOOL  , Bool  , bool       );
  SetJsonByType(STRING, String, std::string);
#undef SetJsonByType
  case FieldDescriptor::CPPTYPE_ENUM:
    if (value.Is<void>()) {
      return true;
    }
    if (!value.CanBe<int>()) {
      return false;
    }
    {
      const EnumValueDescriptor* enum_desc =
          field->enum_type()->FindValueByNumber(value.As<int>());
      if (!enum_desc) {
        // invalid enum value.
        return false;
      }
      refl->SetEnum(&pb, field, enum_desc);
    }
    break;
  case FieldDescriptor::CPPTYPE_MESSAGE:
    if (value.Is<void>()) {
      return true;
    }
    if (!value.CanBe<Mapping>()) {
      return false;
    }
    return Json2Pb(value, *refl->MutableMessage(&pb, field));
  default:
    break;
  }
  return true;
}

} // namespace json
