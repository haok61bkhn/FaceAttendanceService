
#ifndef ENGINE_MODEL_H_
#define ENGINE_MODEL_H_

#include <memory>
#include <opencv2/opencv.hpp>
#include "NvInfer.h"
#include "NvInferPlugin.h"
#include "NvInferRuntime.h"
#include "common.h"
namespace models {

class EngineModel {
 public:
  EngineModel(std::string onnx_file, std::string engine_file,
              int max_batch_size);
  ~EngineModel();
  void LoadEngine();
  // virtual bool InferenceFolder(const std::string& folder_name) = 0;

 protected:
  bool ReadTrtFile();
  void OnnxToTrt();
  void EngineInference(std::vector<float>& input);
  void EngineInference2(std::vector<float>& input, std::vector<int>& numdets,
                        std::vector<float>& boxes, std::vector<float>& scores,
                        std::vector<int>& labels);
  nvinfer1::IRuntime* trtRuntime = nullptr;
  std::string onnx_file_;
  std::string engine_file_;
  int max_batch_size_;
  nvinfer1::IBuilder* builder = nullptr;
  nvinfer1::ICudaEngine* engine_ = nullptr;
  nvinfer1::IExecutionContext* context_ = nullptr;
  std::vector<int64_t> buffer_size_;
  std::vector<void*> buffers_;
  cudaStream_t stream_;
  std::vector<int> out_sizes_;
  int nb_bindings_;
  int input_size_;
  std::vector<std::vector<float>> output_infers_;
  Logger gLogger{nvinfer1::ILogger::Severity::kERROR};
};
}  // namespace models

#endif
