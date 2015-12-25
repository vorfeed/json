#include <chrono>
#include <fstream>
#include <iostream>

#include <gperftools/profiler.h>
#include <json/format.h>
#include "format_rapidjson.h"
#include "video.pb.h"

using namespace std;

int main(int argc, char* argv[]) {
  using json::Video;
  using std::chrono::steady_clock;
  using std::chrono::duration;

  int retry = 1;
  if (argc > 1) {
    retry = atoi(argv[1]);
  }

  ifstream in("../../example/benchmark.json");
  vector<string> json_strs;
  json_strs.resize(101);
  auto ite = json_strs.begin();
  while (!in.eof()) {
    getline(in, *ite++);
  }
  json_strs.pop_back();
  in.close();

  ProfilerStart("benchmark.prof");

  cout << "Json2Pb this" << endl;
  auto start = steady_clock::now();
  vector<Video> video_this;
  video_this.resize(100);
  for (int i : range(retry)) {
    auto ite_this = video_this.begin();
    for (const string& json_str : json_strs) {
      if (!json::Format::Json2Pb(json_str, *ite_this++)) {
        cout << "json2pb failed!" << endl;
      }
    }
  }
  auto end = steady_clock::now();
  duration<double> time_used = end - start;
  cout << "time used: " << time_used.count() << endl << endl;

  cout << "Json2Pb rapidjson" << endl;
  start = steady_clock::now();
  vector<Video> video_rapidjson;
  video_rapidjson.resize(100);
  for (int i : range(retry)) {
    auto ite_rapidjson = video_rapidjson.begin();
    for (const string& json_str : json_strs) {
      if (!Format::Json2Pb(json_str, *ite_rapidjson++)) {
        cout << "json2pb failed!" << endl;
      }
    }
  }
  end = steady_clock::now();
  time_used = end - start;
  cout << "time used: " << time_used.count() << endl << endl;

  cout << "Pb2Json this" << endl;
  start = steady_clock::now();
  for (int i : range(retry)) {
    for (const Video& video : video_this) {
//      cout << json::Format::Pb2Json(video).length() << ", ";
      json::Format::Pb2Json(video);
    }
//    cout << endl;
  }
  end = steady_clock::now();
  time_used = end - start;
  cout << "time used: " << time_used.count() << endl << endl;

  cout << "Pb2Json rapidjson" << endl;
  start = steady_clock::now();
  for (int i : range(retry)) {
    for (const Video& video : video_rapidjson) {
//      cout << Format::Pb2Json(video).length() << ", ";
      Format::Pb2Json(video);
    }
//    cout << endl;
  }
  end = steady_clock::now();
  time_used = end - start;
  cout << "time used: " << time_used.count() << endl << endl;

  ProfilerStop();
}

