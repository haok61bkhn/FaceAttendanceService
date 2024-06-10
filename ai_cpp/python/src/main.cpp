#include <includes/char_detection_yolov8.h>
// #include <includes/detection_yolov8.h>
#include <includes/face_alignment.h>
#include <includes/face_detection.h>
#include <includes/face_extraction.h>
#include <includes/lpn_detection_yolov8.h>
#include <includes/types.h>
#include <pybind11/functional.h>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "includes/face_manager.h"
#include "ndarray_converter.h"

namespace py = pybind11;
PYBIND11_MODULE(py_ai, m) {
  NDArrayConverter::init_numpy();
  py::class_<types::Rect>(m, "Rect")
      .def_readwrite("x", &types::Rect::x)
      .def_readwrite("y", &types::Rect::y)
      .def_readwrite("width", &types::Rect::width)
      .def_readwrite("height", &types::Rect::height);

  py::class_<types::Char_DET>(m, "Char_DET")
      .def_readwrite("rect", &types::Char_DET::rect)
      .def_readwrite("label", &types::Char_DET::label)
      .def_readwrite("prob", &types::Char_DET::prob);

  py::class_<types::DET>(m, "DET")
      .def_readwrite("rect", &types::DET::rect)
      .def_readwrite("label", &types::DET::label)
      .def_readwrite("prob", &types::DET::prob);

  py::class_<types::LPN_DET>(m, "LPN_DET")
      .def_readwrite("rect", &types::LPN_DET::rect)
      .def_readwrite("kps", &types::LPN_DET::kps)
      .def_readwrite("label", &types::LPN_DET::label)
      .def_readwrite("prob", &types::LPN_DET::prob);

  py::class_<types::Point>(m, "Point")
      .def_readwrite("x", &types::Point::x)
      .def_readwrite("y", &types::Point::y)
      .def_readwrite("prob", &types::Point::prob);

  py::class_<types::FaceDetectRes>(m, "FaceDetectRes")
      .def_readwrite("x1", &types::FaceDetectRes::x1)
      .def_readwrite("y1", &types::FaceDetectRes::y1)
      .def_readwrite("x2", &types::FaceDetectRes::x2)
      .def_readwrite("y2", &types::FaceDetectRes::y2)
      .def_readwrite("score", &types::FaceDetectRes::score)
      .def_readwrite("aligned_face", &types::FaceDetectRes::aligned_face)
      .def_readwrite("landmark", &types::FaceDetectRes::landmark);

  py::class_<types::FaceSearchResult>(m, "FaceSearchResult")
      .def_readwrite("pid", &types::FaceSearchResult::pid)
      .def_readwrite("min_distance", &types::FaceSearchResult::min_distance);

  py::class_<models::FaceDetection>(m, "FaceDetection")
      .def(py::init<std::string&, std::string&, int, int, int, int, float,
                    float>(),
           py::arg("onnx_file"), py::arg("engine_file"), py::arg("batch_size"),
           py::arg("input_width"), py::arg("input_height"),
           py::arg("input_chanel"), py::arg("obj_threshold"),
           py::arg("nms_threshold"))
      .def("Demo", &models::FaceDetection::Demo, py::arg("img"))
      .def("Detect", &models::FaceDetection::Detect, py::arg("img"));

  py::class_<models::FaceExtraction>(m, "FaceExtraction")
      .def(py::init<std::string&, std::string&, int, int, int, int>(),
           py::arg("onnx_file"), py::arg("engine_file"), py::arg("batch_size"),
           py::arg("input_width"), py::arg("input_height"),
           py::arg("input_chanel"))
      .def("ExtractFace", &models::FaceExtraction::ExtractFace,
           py::arg("imgs"));

  py::class_<models::FaceAligner>(m, "FaceAligner")
      .def(py::init<>())
      .def("AlignFace", &models::FaceAligner::AlignFace, py::arg("img"),
           py::arg("landmark"));

  py::class_<models::CHAR_DETECTION_YOLOV8>(m, "CHAR_DETECTION_YOLOV8")
      .def(py::init<std::string&, std::string&, std::string&, int, int, int,
                    int>(),
           py::arg("onnx_file"), py::arg("engine_file"),
           py::arg("classes_file"), py::arg("batch_size"),
           py::arg("input_width"), py::arg("input_height"),
           py::arg("input_chanel"))
      .def("Detect", &models::CHAR_DETECTION_YOLOV8::Detect, py::arg("img"))
      .def("DrawChar", &models::CHAR_DETECTION_YOLOV8::DrawChar,
           py::arg("draw_img"), py::arg("detections"))
      .def("GetTextLPN", &models::CHAR_DETECTION_YOLOV8::GetTextLPN,
           py::arg("detections"), py::arg("image_height"));

  py::class_<models::LPN_DETECTION_YOLOV8>(m, "LPN_DETECTION_YOLOV8")
      .def(py::init<std::string&, std::string&, int, int, int, int, int, float,
                    float>(),
           py::arg("onnx_file"), py::arg("engine_file"), py::arg("batch_size"),
           py::arg("input_width"), py::arg("input_height"),
           py::arg("input_chanel"), py::arg("topk"), py::arg("score_thres"),
           py::arg("iou_thres"))
      .def("Detect", &models::LPN_DETECTION_YOLOV8::Detect, py::arg("img"))
      .def("Align_LPN", &models::LPN_DETECTION_YOLOV8::Align_LPN,
           py::arg("img"), py::arg("kps"))
      .def("DrawLPN", &models::LPN_DETECTION_YOLOV8::DrawLPN,
           py::arg("draw_img"), py::arg("detections"));

  py::class_<models::FaceManager>(m, "FaceManager")
      .def(py::init<std::string&, int, int, int, float>(), py::arg("save_path"),
           py::arg("max_size"), py::arg("topk"), py::arg("dim"),
           py::arg("threshold"))
      .def("Insert", &models::FaceManager::Insert, py::arg("feature"),
           py::arg("pid"))
      .def("Search", &models::FaceManager::Search, py::arg("feature"))
      .def("DeleteByPId", &models::FaceManager::DeleteByPId, py::arg("pid"))
      .def("GetNumDatas", &models::FaceManager::GetNumDatas);
  m.doc() = R"pbdoc(
            python wrapper
            -----------------------
            .. currentmodule:: AIpybind
            .. autosummary::
               :toctree: _generate
        )pbdoc";

#ifdef VERSION_INFO
  m.attr("__version__") = VERSION_INFO;
#else
  m.attr("__version__") = "dev";
#endif
}
