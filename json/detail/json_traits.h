// Copyright 2015, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/json
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#pragma once

#ifndef JSON_JSON_H
#error "This should only be included by json.h"
#endif

namespace json {

template <class T> using enable_if_void =
    typename std::enable_if<std::is_void<T>::value>::type;

template <class T> using enable_if_boolean =
    typename std::enable_if<std::is_same<bool, typename std::decay<T>::type>::value>::type;

template <class T> using enable_if_signed_integer =
    typename std::enable_if<
      std::is_integral<typename std::decay<T>::type>::value &&
      std::is_signed<typename std::decay<T>::type>::value
    >::type;

template <class T> using enable_if_unsigned_integer =
    typename std::enable_if<
      std::is_integral<typename std::decay<T>::type>::value &&
      std::is_unsigned<typename std::decay<T>::type>::value &&
      !std::is_same<bool, typename std::decay<T>::type>::value
    >::type;

template <class T> using enable_if_float =
    typename std::enable_if<std::is_floating_point<typename std::decay<T>::type>::value>::type;

template <class T> using enable_if_numeric =
    typename std::enable_if<std::is_arithmetic<typename std::decay<T>::type>::value>::type;

template <class T> using enable_if_string =
    typename std::enable_if<std::is_convertible<T, std::string>::value>::type;

template <class T> using enable_if_sequence =
    typename std::enable_if<std::is_same<Sequence, typename std::decay<T>::type>::value>::type;

template <class T> using enable_if_mapping =
    typename std::enable_if<std::is_same<Mapping, typename std::decay<T>::type>::value>::type;

// TODO: array
template <class T> using enable_if_sequence_like =
    typename std::enable_if<
      std::is_constructible<Json, typename T::value_type>::value &&
      std::is_same<typename std::iterator_traits<typename T::iterator>::iterator_category, std::random_access_iterator_tag>::value &&
      !std::is_same<Sequence, typename std::decay<T>::type>::value &&
      !std::is_convertible<T, std::string>::value
    >::type;

template <class T> using enable_if_mapping_like =
    typename std::enable_if<
      std::is_constructible<std::string, typename T::key_type>::value &&
      std::is_constructible<Json, typename T::mapped_type>::value &&
      !std::is_same<Mapping, typename std::decay<T>::type>::value
    >::type;

template <class T>
struct TypeTraitsBase {
};

template <class T>
struct Json::TypeTraits<T, enable_if_void<T>> : public TypeTraitsBase<T> {
//  typedef void inner_type;
  static constexpr JsonType json_type = EMPTY;
  static constexpr JsonType convertable_type = EMPTY;
};

template <class T>
struct Json::TypeTraits<T, enable_if_boolean<T>> : public TypeTraitsBase<T> {
//  typedef bool inner_type;
  static constexpr JsonType json_type = BOOLEAN;
  static constexpr JsonType convertable_type = static_cast<JsonType>(BOOLEAN | SIGNED_INTEGER | UNSIGNED_INTEGER | FLOAT);
  static void value(Json& json, T value) { json.b_ = value; }
  static bool& value(Json& json) { return json.b_; }
  static T convert(const Json& json) { return value(const_cast<Json&>(json)); }
  static void init(Json& json) { json.b_ = false; }
};

template <class T>
struct Json::TypeTraits<T, enable_if_signed_integer<T>> : public TypeTraitsBase<T> {
//  typedef int64_t inner_type;
  static constexpr JsonType json_type = SIGNED_INTEGER;
  static constexpr JsonType convertable_type = static_cast<JsonType>(BOOLEAN | SIGNED_INTEGER | UNSIGNED_INTEGER | FLOAT);
  static void value(Json& json, T value) { json.i_ = value; }
  static int64_t& value(Json& json) { return json.i_; }
  static T convert(const Json& json) { return value(const_cast<Json&>(json)); }
  static void init(Json& json) { json.i_ = 0; }
};

template <class T>
struct Json::TypeTraits<T, enable_if_unsigned_integer<T>> : public TypeTraitsBase<T> {
//  typedef uint64_t inner_type;
  static constexpr JsonType json_type = UNSIGNED_INTEGER;
  static constexpr JsonType convertable_type = static_cast<JsonType>(BOOLEAN | SIGNED_INTEGER | UNSIGNED_INTEGER | FLOAT);
  static void value(Json& json, T value) { json.u_ = value; }
  static uint64_t& value(Json& json) { return json.u_; }
  static T convert(const Json& json) { return value(const_cast<Json&>(json)); }
  static void init(Json& json) { json.u_ = 0u; }
};

template <class T>
struct Json::TypeTraits<T, enable_if_float<T>> : public TypeTraitsBase<T> {
//  typedef double inner_type;
  static constexpr JsonType json_type = FLOAT;
  static constexpr JsonType convertable_type = static_cast<JsonType>(BOOLEAN | SIGNED_INTEGER | UNSIGNED_INTEGER | FLOAT);
  static void value(Json& json, T value) { json.d_ = value; }
  static double& value(Json& json) { return json.d_; }
  static T convert(const Json& json) { return value(const_cast<Json&>(json)); }
  static void init(Json& json) { json.d_ = 0.; }
};

template <class T>
struct Json::TypeTraits<T, enable_if_string<T>> : public TypeTraitsBase<T> {
//  typedef std::string inner_type;
  static constexpr JsonType json_type = STRING;
  static constexpr JsonType convertable_type = STRING;
  template <class U> static void value(Json& json, U&& value) { json.s_ = new std::string(std::forward<U>(value)); }
  static std::string& value(Json& json) { return *json.s_; }
  static const std::string& convert(const Json& json) { return *json.s_; }
  static void init(Json& json) { json.s_ = new std::string(); }
};

template <class T>
struct Json::TypeTraits<T, enable_if_sequence<T>> : public TypeTraitsBase<T> {
//  typedef Sequence inner_type;
  static constexpr JsonType json_type = SEQUENCE;
  static constexpr JsonType convertable_type = SEQUENCE;
  template <class U> static void value(Json& json, U&& value) { json.a_ = new Sequence(std::forward<U>(value)); }
  static Sequence& value(Json& json) { return *json.a_; }
  static const Sequence& convert(const Json& json) { return *json.a_; }
  static void init(Json& json) { json.a_ = new Sequence(); }
};

template <class T>
struct Json::TypeTraits<T, enable_if_mapping<T>> : public TypeTraitsBase<T> {
//  typedef Mapping inner_type;
  static constexpr JsonType json_type = MAPPING;
  static constexpr JsonType convertable_type = MAPPING;
  template <class U> static void value(Json& json, U&& value) { json.m_ = new Mapping(std::forward<U>(value)); }
  static Mapping& value(Json& json) { return *json.m_; }
  static const Mapping& convert(const Json& json) { return *json.m_; }
  static void init(Json& json) { json.m_ = new Mapping(); }
};

template <class T>
struct Json::TypeTraits<T, enable_if_sequence_like<T>> : public TypeTraitsBase<T> {
  static constexpr JsonType json_type = SEQUENCE;
  static void value(Json& json, const T& value) { json.a_ = new Sequence(value.begin(), value.end()); }
};

template <class T>
struct Json::TypeTraits<T, enable_if_mapping_like<T>> : public TypeTraitsBase<T> {
  static constexpr JsonType json_type = MAPPING;
  static void value(Json& json, const T& value) { json.m_ = new Mapping(value.begin(), value.end()); }
};

} // namespace json
