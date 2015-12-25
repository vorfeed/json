#include <json/json.h>

#include <deque>
#include <iostream>
#include <map>

using namespace std;

int main(int argc, char* argv[]) {
  using json::Json;
  using json::Sequence;
  using json::Mapping;

  Json v;
  cout << "v is void: " << v.Is<void>() << endl;
  cout << "v: " << v.str() << endl;
  Json b(true);
  cout << "b is bool: " << b.Is<bool>() << endl;
  cout << "b: " << b.str() << endl;
  Json i(1);
  cout << "i is signed: " << i.Is<signed>() << endl;
  cout << "i is int32_t: " << i.Is<int32_t>() << endl;
  cout << "i is int64_t: " << i.Is<int64_t>() << endl;
  cout << "i: " << i.str() << endl;
  Json u(1u);
  cout << "u is unsigned: " << u.Is<unsigned>() << endl;
  cout << "u is uint32_t: " << u.Is<uint32_t>() << endl;
  cout << "u is uint64_t: " << u.Is<uint64_t>() << endl;
  cout << "u: " << u.str() << endl;
  Json d(1.);
  cout << "d is float: " << d.Is<float>() << endl;
  cout << "d is double: " << d.Is<double>() << endl;
  cout << "d: " << d.str() << endl;
  Json s("1");
  cout << "s is string: " << s.Is<string>() << endl;
  cout << "s: " << s.str() << endl;
  Json a(Sequence { 1, 2 });
  cout << "a is sequence: " << a.Is<Sequence>() << endl;
  cout << "a: " << a.str() << endl;
  Json m(Mapping { { "1", 1 }, { "2", "2" } });
  cout << "m is mapping: " << m.Is<Mapping>() << endl;
  cout << "m: " << m.str() << endl;
  cout << endl;

  cout << "b as bool: " << b.As<bool>() << endl;
  cout << "i as signed: " << i.As<signed>() << endl;
  cout << "i as int32_t: " << i.As<int32_t>() << endl;
  cout << "i as int64_t: " << i.As<int64_t>() << endl;
  cout << "u as unsigned: " << u.As<unsigned>() << endl;
  cout << "u as uint32_t: " << u.As<uint32_t>() << endl;
  cout << "u as uint64_t: " << u.As<uint64_t>() << endl;
  cout << "d as float: " << d.As<float>() << endl;
  cout << "d as double: " << d.As<double>() << endl;
  cout << "s as string: " << s.As<string>() << endl;
  cout << "a as sequence: " << to_string(a.As<Sequence>()) << endl;
  cout << "m as mapping: " << to_string(m.As<Mapping>()) << endl;
  cout << endl;

  b.As<bool>() = false;
  cout << "b is false: " << b.str() << endl;
  i.As<int64_t>() = 2;
  cout << "i is 2: " << i.str() << endl;
  u.As<uint64_t>() = 2u;
  cout << "u is 2: " << u.str() << endl;
  d.As<double>() = 2.;
  cout << "d is 2: " << d.str() << endl;
  s.As<string>() = "2";
  cout << "s is 2: " << s.str() << endl;
  a.As<Sequence>() = { 2, 3 };
  cout << "a is 2,3: " << a.str() << endl;
  m.As<Mapping>() = { { "2", 2 }, { "3", "3" } };
  cout << "m is 2,3: " << m.str() << endl;
  cout << endl;

  try {
    cout << "b as signed: " << b.As<signed>() << endl;
  } catch (const std::exception& e) {
    cout << "b as signed failed: " << e.what() << endl;
  }
  try {
  cout << "b as unsigned: " << b.As<unsigned>() << endl;
  } catch (const std::exception& e) {
    cout << "b as unsigned failed: " << e.what() << endl;
  }
  try {
  cout << "b as float: " << b.As<float>() << endl;
  } catch (const std::exception& e) {
    cout << "b as float failed: " << e.what() << endl;
  }
  try {
  cout << "b as string: " << b.As<string>() << endl;
  } catch (const std::exception& e) {
    cout << "b as string failed: " << e.what() << endl;
  }
  try {
  cout << "b as sequence: " << to_string(b.As<Sequence>()) << endl;
  } catch (const std::exception& e) {
    cout << "b as sequence failed: " << e.what() << endl;
  }
  try {
  cout << "b as mapping: " << to_string(b.As<Mapping>()) << endl;
  } catch (const std::exception& e) {
    cout << "b as mapping failed: " << e.what() << endl;
  }
  cout << endl;

  v = true;
  cout << "v is bool: " << v.Is<bool>() << endl;
  v = 1;
  cout << "v is signed: " << v.Is<signed>() << endl;
  v = 1u;
  cout << "v is unsigned: " << v.Is<unsigned>() << endl;
  v = 1.;
  cout << "v is float: " << v.Is<float>() << endl;
  v = "1";
  cout << "v is string: " << v.Is<string>() << endl;
  v = Sequence { 1, 2 };
  cout << "v is sequence: " << v.Is<Sequence>() << endl;
  v = Mapping { { "1", 1 }, { "2", "2" } };
  cout << "v is mapping: " << v.Is<Mapping>() << endl;
  cout << endl;

  Json alike(deque<int> { 1, 2 });
  cout << "sequence like: " << alike.str() << endl;
  v = deque<int> { 1, 2 };
  cout << "sequence like assignment: " << v.str() << endl;
  Json mlike(map<string, int> { { "1", 1 }, { "2", 2 } });
  cout << "mapping like: " << mlike.str() << endl;
  v = map<string, int> { { "1", 1 }, { "2", 2 } };
  cout << "mapping mlike assignment: " << v.str() << endl;
  cout << endl;

  Json json(Mapping {
    { "key1", 1 },
    { "key2", Sequence { Mapping { {"key21", 21} }, Mapping { {"key22", 22} } } },
    { "key3", Mapping { { "key31", "31" }, { "key32", Sequence { Mapping { { "key321", 321 } }, Mapping { { "key322", 322 } } } } } },
  });
  cout << "json is mapping: " << json.Is<Mapping>() << endl;
  cout << "json: " << json.str() << endl;
  v = Mapping {
    { "key1", 1 },
    { "key2", Sequence { Mapping { {"key21", 21} }, Mapping { {"key22", 22} } } },
    { "key3", Mapping { { "key31", "31" }, { "key32", Sequence { Mapping { { "key321", 321 } }, Mapping { { "key322", 322 } } } } } },
  };
  cout << "v from assign is mapping: " << v.Is<Mapping>() << endl;
  cout << "v from assign: " << json.str() << endl;
  cout << endl;

  Json json_copy(json);
  cout << "json_copy is mapping: " << json_copy.Is<Mapping>() << endl;
  cout << "json_copy: " << json_copy.str() << endl;
  Json json_move(std::move(json));
  cout << "json_move is mapping: " << json_move.Is<Mapping>() << endl;
  cout << "json_move: " << json_move.str() << endl;
  cout << "json after move is void: " << json.Is<void>() << endl;
  cout << "json after move: " << json.str() << endl;
  json = json_copy;
  cout << "json from assign is mapping: " << json.Is<Mapping>() << endl;
  cout << "json from assign: " << json.str() << endl;
  json = std::move(json_move);
  cout << "json from assign move is mapping: " << json.Is<Mapping>() << endl;
  cout << "json from assign move: " << json.str() << endl;
  cout << endl;

  cout << "json[\"key2\"][1]: " << json["key2"][1].str() << endl;
  cout << "json[\"key2\"][1][\"key22\"]: " << json["key2"][1]["key22"].str() << endl;
  cout << "json[\"key3\"][\"key32\"]: " << json["key3"]["key32"].str() << endl;
  cout << "json[\"key3\"][\"key32\"][1][\"key322\"]: " << json["key3"]["key32"][1]["key322"].str() << endl;
  cout << endl;

  Json obj(Json::Create("{\"key3\":{\"key32\":[{\"key321\":321},{\"key322\":322}],\"key31\":\"31\"},\"key1\":1,\"key2\":[{\"key21\":21},{\"key22\":22}]}"));
  cout << "json object: " << obj.str() << endl;
  cout << endl;

  json = Sequence { 1, 2 };
  cout << "json sequence values: " << json[0] << "," << json[1] << endl;
  try {
    json[2];
  } catch (const std::exception& e) {
    cout << "get json sequence value failed: " << e.what() << endl;
  }
  cout << endl;

  json = Mapping { { "1", 1 }, { "2", "2" } };
  cout << "json mapping values: " << json["1"] << "," << json["2"] << endl;
  cout << "not exist json mapping value: " << json["3"] << endl;
  json["3"] = Sequence { 1, 2 };
  cout << "new json mapping value: " << json["3"].str() << endl;
  cout << endl;

  try {
    b[1];
  } catch (const std::exception& e) {
    cout << "can't convert boolean to sequence: " << e.what() << endl;
  }
  try {
    b["1"];
  } catch (const std::exception& e) {
    cout << "can't convert boolean to mapping: " << e.what() << endl;
  }
  b.To<Mapping>();
  b["1"] = 1;
  cout << "mapping b converted from boolean: " << b["1"] << endl;
  cout << endl;

  Json j = Sequence { 1, 2, 3};
  auto s1 = j.As<Sequence>();
  auto& s2 = j.As<Sequence>();
  const Sequence& s3 = j.As<Sequence>();
  cout << "s1: " << s1.data() << endl;
  cout << "s2: " << s2.data() << endl;
  cout << "s3: " << s3.data() << endl;
  cout << "j as sequence: " << j.As<Sequence>().data() << endl;
  Sequence& s4 = j.As<Sequence>();
  s4[0] = 4;
  cout << reinterpret_cast<const void*>(s3.data()) << endl;
  cout << "s1 after modify: " << s1[0] << endl;
  cout << "s1 after modify: " << s2[0] << endl;
  cout << "s1 after modify: " << s3[0] << endl;
  cout << "j as sequence after modify: " << j.As<Sequence>().data() << endl;
  cout << endl;
}

