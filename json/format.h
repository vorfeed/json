// Copyright 2015, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/json
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#pragma once

#ifndef JSON_FORMAT_H
#define JSON_FORMAT_H

#include <string>
#include <sstream>
#include <unordered_map>

#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

#include "json.h"
#include "range.h"

namespace json {

using google::protobuf::Message;
using google::protobuf::Descriptor;
using google::protobuf::FieldDescriptor;
using google::protobuf::Reflection;
using google::protobuf::EnumValueDescriptor;

class Format {
 public:
  static std::string Pb2Json(const Message& pb);
  static Json& Pb2Json(const Message& pb, Json& json);

  static bool Json2Pb(const std::string& json, Message& pb);
  static bool Json2Pb(const Json& json, Message& pb);

 private:
  static inline void AddRepeatedJson(const Message& pb,
      const FieldDescriptor* field, const Reflection* refl, Json& json);
  static inline void AddNonrepeatedJson(const Message& pb,
      const FieldDescriptor* field, const Reflection* refl, Json& json);

  static inline bool AddJson(const Json& value, const FieldDescriptor* field,
      const Reflection* refl, Message& pb);
  static inline bool SetJson(const Json& value, const FieldDescriptor* field,
      const Reflection* refl, Message& pb);
};

} // namespace json

#include "detail/format-inl.h"

#endif // JSON_FORMAT_H
