#include "DataProducer.h"

DataProducer::DataProducer(std::string& url) {
  Configuration config = {{"metadata.broker.list", url}};
  producer = new Producer(config);
}

DataProducer::~DataProducer() { delete producer; }

void DataProducer::ProduceData(std::vector<str_data>& str_data_list,
                               std::vector<int_data>& int_data_list,
                               std::vector<float_data>& float_data_list,
                               std::string& topic) {
  string data;
  ConvertToJson(str_data_list, int_data_list, float_data_list, data);
  producer->produce(MessageBuilder(topic).partition(0).payload(data));
}

void DataProducer::ConvertToJson(std::vector<str_data>& str_data_list,
                                 std::vector<int_data>& int_data_list,
                                 std::vector<float_data>& float_data_list,
                                 string& json) {
  json = "{";
  for (auto& str_data : str_data_list) {
    json += "\"" + str_data.key + "\":\"" + str_data.value + "\",";
  }
  for (auto& int_data : int_data_list) {
    json += "\"" + int_data.key + "\":" + std::to_string(int_data.value) + ",";
  }
  for (auto& float_data : float_data_list) {
    json +=
        "\"" + float_data.key + "\":" + std::to_string(float_data.value) + ",";
  }
  json.pop_back();
  json += "}";
}
