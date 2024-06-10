import os

import cv2
import py_ai
import yaml

MIN_WIDTH_FACE = 50
MIN_HEIGHT_FACE = 50
MIN_SCORE_FACE = 0.5
NUM_FACE_PROCESSES = 1
NUM_FACE_UPDATE_THREADS = 4
FACE_MAX_SIZE_QUEUE = 1000
AI_DATA_PATH = "resource/ai_data"


class FaceAI:
    def __init__(self):
        self.init_face_detector()
        self.init_face_extractor()

    def init_face_detector(self):
        face_detection_config = os.path.join(
            AI_DATA_PATH, "config/face_detection_py.yaml"
        )
        with open(face_detection_config, "r") as f:
            config = yaml.load(f, Loader=yaml.FullLoader)["SCRFD"]
            self.face_detector = py_ai.FaceDetection(
                os.path.join(AI_DATA_PATH, config["onnx_file"]),
                os.path.join(AI_DATA_PATH, config["engine_file"]),
                config["BATCH_SIZE"],
                config["IMAGE_WIDTH"],
                config["IMAGE_HEIGHT"],
                config["INPUT_CHANNEL"],
                config["obj_threshold"],
                config["nms_threshold"],
            )
        self.face_aligner = py_ai.FaceAligner()

    def init_face_extractor(self):
        face_extraction_config = os.path.join(
            AI_DATA_PATH, "config/face_extraction_py.yaml"
        )
        with open(face_extraction_config, "r") as f:
            config = yaml.load(f, Loader=yaml.FullLoader)["IRESNET"]
            self.face_extractor = py_ai.FaceExtraction(
                os.path.join(AI_DATA_PATH, config["onnx_file"]),
                os.path.join(AI_DATA_PATH, config["engine_file"]),
                config["BATCH_SIZE"],
                config["IMAGE_WIDTH"],
                config["IMAGE_HEIGHT"],
                config["INPUT_CHANNEL"],
            )

    def detect_image_paths(self, image_paths, padding=20):
        images = [cv2.imread(path) for path in image_paths]
        return self.detect_images(images, padding)

    def detect_images(self, images, padding=20):
        cropped_faces = []
        features = []
        for image in images:
            face_dets = self.face_detector.Detect(image)
            for face_det in face_dets:
                x1 = face_det.x1
                x2 = face_det.x2
                y1 = face_det.y1
                y2 = face_det.y2
                width = x2 - x1
                height = y2 - y1
                score = face_det.score
                if (
                    width > MIN_WIDTH_FACE
                    and height > MIN_HEIGHT_FACE
                    and score > MIN_SCORE_FACE
                ):
                    landmark = face_det.landmark
                    aligned_face = self.face_aligner.AlignFace(image, landmark)
                    feature = self.face_extractor.ExtractFace([aligned_face])[0]
                    x1 = int(max(0, x1 - padding))
                    y1 = int(max(0, y1 - padding))
                    x2 = int(min(image.shape[1], x2 + padding))
                    y2 = int(min(image.shape[0], y2 + padding))
                    cropped_face = image[y1:y2, x1:x2]
                    cropped_faces.append(cropped_face)
                    features.append(feature)
        return [cropped_faces, features]

    def get_feature_face(self, image, check_one_face=True, padding=20):
        max_area = 0
        face_det_max_area = None
        face_dets = self.face_detector.Detect(image)
        if check_one_face and len(face_dets) != 1:
            return None
        for face_det in face_dets:
            x1 = face_det.x1
            x2 = face_det.x2
            y1 = face_det.y1
            y2 = face_det.y2
            width = x2 - x1
            height = y2 - y1
            if width * height > max_area:
                max_area = width * height
                face_det_max_area = face_det
        if face_det_max_area is None:
            return None
        aligned_face = self.face_aligner.AlignFace(image, face_det_max_area.landmark)
        feature = self.face_extractor.ExtractFace([aligned_face])[0]
        x1 = int(max(0, face_det_max_area.x1 - padding))
        y1 = int(max(0, face_det_max_area.y1 - padding))
        x2 = int(min(image.shape[1], face_det_max_area.x2 + padding))
        y2 = int(min(image.shape[0], face_det_max_area.y2 + padding))
        cropped_face = image[y1:y2, x1:x2]
        return [cropped_face, feature]
