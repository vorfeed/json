# Json

A C++ Library for Parsing and Serializing Json

Conversion between Json and Protobuf


## Features

Json

* Parse json string into Json object, with vector and unordered_map

Format

* Convert between Json and protobuf

* Also provide a conversion between the thirdparty rapidjson object and protobuf


## Example

create a json object

* Json json;

* Json array_like(std::deque<int> { 1, 2 });

* Json mapping_like(std::map<std::string, int> { { "1", 1 }, { "2", 2 } });

* complex object
  Json json(Mapping {
    { "1", 1 },
    { "2", Sequence { Mapping { {"21", 21} }, Mapping { {"22", 22} } } },
    { "3", Mapping { { "31", "31" }, { "32", Sequence { Mapping { { "321", 321 } }, Mapping { { "322", 322 } } } } } },
  });

copy and assign operation

* json = std::vector<Json> { 1, 2 };

* json = std::unordered_map<std::string, Json> { { "1", 1 }, { "2", "2" } };

* Json json_copy(json);

* Json json_move(std::move(json));

* json = json_copy;

* json = std::move(json_move);

check json type

* json.Is<int32_t>();

get and set type value

* int32_t value = json.As<int32_t>();

* json.As<int32_t>() = 123;

operator []

* json["key3"]["key32"][1]["key322"].str()

parse from string

* Json obj(Json::Create("{\"3\":{\"32\":[{\"321\":321},{\"322\":322}],\"31\":\"31\"},\"1\":1,\"2\":[{\"21\":21},{\"22\":22}]}"));

serialize to string

* std::cout << to_string(json.As<Sequence>()) << std::endl;

* std::cout << json.str() << std::endl;

* std::stringstream ss;  ss << json;

convert type

* Json b(true);  b.To<Mapping>();  b["1"] = 1;

return value or reference

> Json j = Sequence { 1, 2, 3};
> auto s1 = j.As<Sequence>();
> auto& s2 = j.As<Sequence>();
> const Sequence& s3 = j.As<Sequence>();
> // 0x1395ab0, 0x1395b60, 0x1395b60
> std::cout << s1.data() << s2.data() << s3.data() << std::endl;
> // 0x1395b60
> std::cout << j.As<Sequence>().data() << std::endl;

> Sequence& s4 = j.As<Sequence>();
> s4[0] = 4;
> // 0x1395b60
> std::cout << reinterpret_cast<const void*>(s3.data()) << std::endl;
> // 1, 4, 4
> std::cout << s1[0] << s2[0] << s3[0] << std::endl;
> // 0x1395b60
> std::cout << j.As<Sequence>().data() << std::endl;


## Protobuf Conversion Example

* json string to protobuf object
> Video video;
> Format::Json2Pb(json_str, video);

* protobuf object to json string
> std::cout << Format::Pb2Json(video) << std::endl;


## Dependings

* cmake

* optional: protobuf


================================
by Xiaojie Chen (swly@live.com)

