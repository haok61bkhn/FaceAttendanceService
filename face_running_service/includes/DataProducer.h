#ifndef DATA_PRODUCER_H
#define DATA_PRODUCER_H

#include <cppkafka/cppkafka.h>
#include <iostream>
#include <string>

using namespace std;
using namespace cppkafka;

struct str_data {
  str_data(string key, string value) : key(key), value(value) {}
  string key;
  string value;
};
struct int_data {
  int_data(string key, int value) : key(key), value(value) {}
  string key;
  int value;
};
struct float_data {
  float_data(string key, float value) : key(key), value(value) {}
  string key;
  float value;
};
class DataProducer {
 public:
  DataProducer(std::string& url);
  ~DataProducer();
  void ProduceData(std::vector<str_data>& str_data_list,
                   std::vector<int_data>& int_data_list,
                   std::vector<float_data>& float_data_list,
                   std::string& topic);

 private:
  void ConvertToJson(std::vector<str_data>& str_data_list,
                     std::vector<int_data>& int_data_list,
                     std::vector<float_data>& float_data_list, string& json);

 private:
  Producer* producer;
};
#endif  // DATA_PRODUCER_H