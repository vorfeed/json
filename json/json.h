// Copyright 2015, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/json
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#pragma once

#ifndef JSON_JSON_H
#define JSON_JSON_H

#include <cassert>
#include <cstdlib>

#include <bitset>
#include <initializer_list>
#include <iostream>
#include <limits>
#include <memory>
#include <sstream>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace json {

class Json;
typedef std::vector<Json> Sequence;
typedef std::unordered_map<std::string, Json> Mapping;

class Json {
  template <class T> using enable_if_not_json =
      typename std::enable_if<!std::is_same<typename std::decay<T>::type, Json>::value>::type;

  template <class T, class Enable = void> struct TypeTraits;

 public:
  static Json Create(const std::string& json);

  Json() noexcept;
//  template <class T> Json();

  template <class T, enable_if_not_json<T>* = nullptr> Json(T&& value);
  template <class T, enable_if_not_json<T>* = nullptr> Json& operator=(T&& value);

  Json(const Json& other);
  Json& operator=(const Json& other);

  Json(Json&& other) noexcept;
  Json& operator=(Json&& other) noexcept;

  ~Json();

  template <class T> bool Is() const;

  template <class T> bool CanBe() const;

  template <class T> auto As() const ->
      typename std::result_of<decltype(&TypeTraits<T>::convert)(const Json&)>::type;
  template <class T> auto& As();

  template <class T> T& To();

  const Json& operator[](size_t index) const;
  Json& operator[](size_t index);

  Json& operator[](const std::string& key);
  Json& operator[](std::string&& key);  // if necessary?

  void Clear();

  std::string str() const;

  friend std::ostream& operator<<(std::ostream& out, const Json& value);
  friend std::ostream& operator<<(std::ostream& out, const Sequence& values);
  friend std::ostream& operator<<(std::ostream& out, const Mapping& values);

  template <class T> friend std::string to_string(const T& value);

  bool operator==(const Json& rhs) const;

 private:
  class Parser;

  enum JsonType {
    EMPTY = 0,
    BOOLEAN = 1, SIGNED_INTEGER = 2, UNSIGNED_INTEGER = 4, FLOAT = 8,
    STRING = 16,
    SEQUENCE = 32,
    MAPPING = 64,
  };

  static void Escape(std::ostream& out, const std::string& str);

  JsonType type_;
  union {
    bool b_;
    int64_t i_;
    uint64_t u_;
    double d_;
    std::string* s_;
    Sequence* a_;
    Mapping* m_;
  };
};

} // namespace json

#include "detail/json_traits.h"
#include "detail/json_parser.h"
#include "detail/json-inl.h"

#endif // JSON_JSON_H
