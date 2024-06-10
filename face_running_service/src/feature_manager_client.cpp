#include "feature_manager_client.h"

FeaturesManagerClient::FeaturesManagerClient(
    std::shared_ptr<grpc::Channel> channel)
    : stub_(featuresmanager::FeaturesManager::NewStub(channel)) {}

bool FeaturesManagerClient::SearchFeature(std::vector<float>& feature,
                                          std::string group_id, int topk,
                                          std::vector<std::string>& ids,
                                          std::vector<float>& scores) {
  featuresmanager::FeatureSearchingSet request;
  request.set_topk(topk);
  request.set_group_id(group_id);
  request.set_min_score(0.6);
  for (auto& f : feature) {
    request.add_features(f);
  }
  featuresmanager::ResultSearching reply;
  grpc::ClientContext context;
  std::chrono::system_clock::time_point deadline =
      std::chrono::system_clock::now() + std::chrono::milliseconds(10);
  context.set_deadline(deadline);
  grpc::Status status = stub_->SearchFeature(&context, request, &reply);

  if (status.ok()) {
    for (int i = 0; i < reply.ids_size(); i++) {
      ids.push_back(reply.ids(i));
      scores.push_back(reply.scores(i));
    }
    return true;
  } else {
    std::cout << status.error_code() << "  " << status.error_message()
              << std::endl;
    return false;
  }
}
