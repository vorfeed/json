#include <json/format.h>

#include <fstream>
#include <iostream>

#include "video.pb.h"

using namespace std;

int main(int argc, char* argv[]) {
  using json::Json;
  using json::Sequence;
  using json::Mapping;
  using json::Format;
  using json::Video;

  ifstream in("../../example/dmbj.json");
  string json_str;
  getline(in, json_str);
  in.close();

  Video video;
  Format::Json2Pb(json_str, video);

  cout << "Pb2Json: " << endl;
  cout << Format::Pb2Json(video) << endl;
  cout << endl;
}

