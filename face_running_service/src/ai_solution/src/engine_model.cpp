#include "engine_model.h"
namespace models {
EngineModel::EngineModel(std::string onnx_file, std::string engine_file,
                         int max_batch_size) {
  onnx_file_ = onnx_file;
  engine_file_ = engine_file;
  max_batch_size_ = max_batch_size;
}

EngineModel::~EngineModel() {
  cudaStreamDestroy(stream_);
  for (auto& buffer : buffers_) cudaFree(buffer);

  if (context_ != nullptr) context_->destroy();

  if (engine_ != nullptr) engine_->destroy();
  context_ = nullptr;
  engine_ = nullptr;
}
void EngineModel::OnnxToTrt() {
  builder = nvinfer1::createInferBuilder(gLogger.getTRTLogger());
  assert(builder != nullptr);
  const auto explicitBatch =
      1U << static_cast<uint32_t>(
          nvinfer1::NetworkDefinitionCreationFlag::kEXPLICIT_BATCH);
  auto network = builder->createNetworkV2(explicitBatch);
  auto config = builder->createBuilderConfig();

  auto parser = nvonnxparser::createParser(*network, gLogger.getTRTLogger());
  if (!parser->parseFromFile(
          onnx_file_.c_str(),
          static_cast<int>(gLogger.getReportableSeverity()))) {
    gLogError << "Failure while parsing ONNX file" << std::endl;
  }
  builder->setMaxBatchSize(max_batch_size_);
  config->setMaxWorkspaceSize(1_GiB);
  config->setFlag(nvinfer1::BuilderFlag::kFP16);

  std::cout << "start building engine" << std::endl;
  engine_ = builder->buildEngineWithConfig(*network, *config);
  std::cout << "build engine done" << std::endl;
  assert(engine_);
  parser->destroy();
  nvinfer1::IHostMemory* data = engine_->serialize();
  std::ofstream file;
  file.open(engine_file_, std::ios::binary | std::ios::out);
  std::cout << "writing engine file..." << std::endl;
  file.write((const char*)data->data(), data->size());
  std::cout << "save engine file done" << std::endl;
  file.close();
  network->destroy();
  builder->destroy();
}

bool EngineModel::ReadTrtFile() {
  initLibNvInferPlugins(&gLogger.getTRTLogger(), "");
  std::string cached_engine;
  std::fstream file;
  std::cout << "loading filename from:" << engine_file_ << std::endl;
  std::ifstream in(engine_file_.c_str(), std::ifstream::binary);
  if (in.is_open()) {
    auto const start_pos = in.tellg();
    in.ignore(std::numeric_limits<std::streamsize>::max());
    size_t bufCount = in.gcount();
    in.seekg(start_pos);
    std::unique_ptr<char[]> engineBuf(new char[bufCount]);
    in.read(engineBuf.get(), bufCount);
    trtRuntime = nvinfer1::createInferRuntime(gLogger.getTRTLogger());
    engine_ = trtRuntime->deserializeCudaEngine((void*)engineBuf.get(),
                                                bufCount, nullptr);
    assert(engine_ != nullptr);

    trtRuntime->destroy();
    trtRuntime = nullptr;
    std::cout << "deserialize done" << std::endl;
    return true;
  }

  // file.open(engine_file_, std::ios::binary | std::ios::in);
  // if (!file.is_open()) {
  //   std::cout << "read file error: " << engine_file_ << std::endl;
  //   cached_engine = "";
  // }
  // while (file.peek() != EOF) {
  //   std::stringstream buffer;
  //   buffer << file.rdbuf();
  //   cached_engine.append(buffer.str());
  // }
  // file.close();

  // trtRuntime = nvinfer1::createInferRuntime(gLogger.getTRTLogger());
  // engine_ = trtRuntime->deserializeCudaEngine(cached_engine.data(),
  //                                             cached_engine.size(), nullptr);
  // trtRuntime->destroy();

  return false;
}

void EngineModel::LoadEngine() {
  std::fstream existEngine;
  existEngine.open(engine_file_, std::ios::in);
  if (existEngine) {
    bool status = ReadTrtFile();
    assert(engine_ != nullptr);
  } else {
    OnnxToTrt();
    assert(engine_ != nullptr);
  }
  assert(engine_ != nullptr);
  context_ = engine_->createExecutionContext();
  assert(context_ != nullptr);
  nb_bindings_ = engine_->getNbBindings();
  buffer_size_.resize(nb_bindings_);
  buffers_.resize(nb_bindings_);
  out_sizes_.resize(nb_bindings_ - 1);
  output_infers_.resize(nb_bindings_ - 1);
  std::cout << "nb_bindings_:" << nb_bindings_ << std::endl;
  for (int i = 0; i < nb_bindings_; ++i) {
    nvinfer1::Dims dims = engine_->getBindingDimensions(i);
    nvinfer1::DataType dtype = engine_->getBindingDataType(i);
    int64_t totalSize = volume(dims) * 1 * getElementSize(dtype);
    buffer_size_[i] = totalSize;
    cudaMalloc(&buffers_[i], totalSize);
  }
  cudaStreamCreate(&stream_);

  for (int i = 1; i < nb_bindings_; i++) {
    out_sizes_[i - 1] = int(buffer_size_[i] / sizeof(float) / max_batch_size_);
    output_infers_[i - 1].resize(out_sizes_[i - 1] * max_batch_size_);
  };
  input_size_ = int(buffer_size_[0] / sizeof(float) * max_batch_size_);
}

void EngineModel::EngineInference(std::vector<float>& input) {
  assert(engine_ != nullptr);
  assert(context_ != nullptr);

  if (input.size() != input_size_) {
    input.resize(input_size_);
  }
  cudaMemcpyAsync(buffers_[0], input.data(), buffer_size_[0],
                  cudaMemcpyHostToDevice, stream_);
  cudaDeviceSynchronize();
  context_->execute(max_batch_size_, buffers_.data());

  for (int i = 0; i < nb_bindings_ - 1; i++) {
    cudaMemcpyAsync(output_infers_[i].data(), buffers_[i + 1],
                    buffer_size_[i + 1], cudaMemcpyDeviceToHost, stream_);
  }
  cudaStreamSynchronize(stream_);
}

void EngineModel::EngineInference2(std::vector<float>& input,
                                   std::vector<int>& numdets,
                                   std::vector<float>& boxes,
                                   std::vector<float>& scores,
                                   std::vector<int>& labels) {
  assert(engine_ != nullptr);
  assert(context_ != nullptr);

  if (input.size() != input_size_) {
    input.resize(input_size_);
  }
  cudaMemcpyAsync(buffers_[0], input.data(), buffer_size_[0],
                  cudaMemcpyHostToDevice, stream_);
  cudaDeviceSynchronize();
  context_->execute(max_batch_size_, buffers_.data());

  numdets.resize(buffer_size_[1] * max_batch_size_);
  cudaMemcpyAsync(numdets.data(), buffers_[1], buffer_size_[1],
                  cudaMemcpyDeviceToHost, stream_);

  boxes.resize(buffer_size_[2] * max_batch_size_);
  cudaMemcpyAsync(boxes.data(), buffers_[2], buffer_size_[2],
                  cudaMemcpyDeviceToHost, stream_);
  scores.resize(buffer_size_[3] * max_batch_size_);
  cudaMemcpyAsync(scores.data(), buffers_[3], buffer_size_[3],
                  cudaMemcpyDeviceToHost, stream_);
  labels.resize(buffer_size_[4] * max_batch_size_);
  cudaMemcpyAsync(labels.data(), buffers_[4], buffer_size_[4],
                  cudaMemcpyDeviceToHost, stream_);
  cudaStreamSynchronize(stream_);
}
}  // namespace models
