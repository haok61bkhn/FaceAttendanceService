import src.proto_py.deepstream_service_pb2 as deepstream_message
import src.proto_py.deepstream_service_pb2_grpc as deepstream_service
from src.service_client import ServiceClient


class DeepStreamManagerService:
    def __init__(self, host: str, port: int, stub_name: str):
        self.client = ServiceClient(deepstream_service, stub_name, host, port)

    def insert_camera(self, camera_id: str, camera_name: str, camera_url: str):
        response = self.client.InsertCamera(
            deepstream_message.CameraItem(
                camera_id=camera_id, camera_name=camera_name, camera_url=camera_url
            )
        )
        if response:
            return response.status

    def remove_camera(self, camera_id: str):
        response = self.client.RemoveCamera(
            deepstream_message.CameraItem(camera_id=camera_id)
        )
        if response:
            return response.status

    def get_cameras(self):
        response = self.client.GetCameraStatusList(deepstream_message.Empty())
        if response:
            outputs = []
            for item in response.camera_status_list:
                outputs.append(
                    {
                        "camera_id": item.camera_id,
                        "camera_name": item.camera_name,
                        "active": item.active,
                        "fps": item.fps,
                    }
                )
            return outputs
        else:
            return None

    def insert_face(self, face_id: str, face_image_paths: list):
        response = self.client.InsertFace(
            deepstream_message.FaceItem(
                face_id=face_id, face_image_paths=face_image_paths
            )
        )
        if response:
            return (
                response.status,
                list(response.face_crop_image_paths),
                response.message,
            )

    def remove_face(self, face_id: str):
        response = self.client.RemoveFace(deepstream_message.FaceID(face_id=face_id))
        if response:
            return response.status

    def get_face_ids(self):
        response = self.client.GetFaceIDs(deepstream_message.Empty())
        if response:
            return list(response.face_ids)
        else:
            return None


if __name__ == "__main__":
    deepstreammng = DeepStreamManagerService(
        "localhost", 50051, "DeepstreamServiceStub"
    )

    ## Insert Camera
    # camera_id = "camera_test"
    # camera_name = "Camera test"
    # camera_url = "rtsp://12"
    # response = deepstreammng.insert_camera(camera_id, camera_name, camera_url)
    # print("InsertCamera:", response.status)
