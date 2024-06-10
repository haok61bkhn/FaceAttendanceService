import grpc
import sys
from functools import partial
import proto_py.deepstream_service_pb2 as deepstream_message
import proto_py.deepstream_service_pb2_grpc as deepstream_service


class ServiceClient:

    def __init__(self, service_module, stub_name, host, port, timeout=10):
        channel = grpc.insecure_channel("{0}:{1}".format(host, port))
        try:
            grpc.channel_ready_future(channel).result(timeout=10)
        except grpc.FutureTimeoutError:
            sys.exit("Error connecting to server")
        self.stub = getattr(service_module, stub_name)(channel)
        self.timeout = timeout

    def __getattr__(self, attr):
        return partial(self._wrapped_call, self.stub, attr)

    # args[0]: stub, args[1]: function to call, args[3]: Request
    # kwargs: keyword arguments
    def _wrapped_call(self, *args, **kwargs):
        try:
            return getattr(args[0], args[1])(args[2], **kwargs, timeout=self.timeout)
        except grpc.RpcError as e:
            print("Call {0} failed with {1}".format(args[1], e.code()))
            raise


if __name__ == "__main__":
    deepstream_manager = ServiceClient(
        deepstream_service, "DeepstreamServiceStub", "localhost", 50051
    )

    # # Insert a camera
    # response = deepstream_manager.InsertCamera(
    #     deepstream_message.CameraItem(
    #         camera_id="test", using_lp_recognition=True, using_face_recognition=True
    #     )
    # )

    # if response:
    #     print("status :", response.status)

    # # Remove a camera
    # response = deepstream_manager.RemoveCamera(
    #     deepstream_message.CameraItem(camera_id="test")
    # )
    # if response:
    #     print("status :", response.status)

    # # Update a camera
    # response = deepstream_manager.UpdateCamera(
    #     deepstream_message.CameraItem(
    #         camera_id="test", using_lp_recognition=True, using_face_recognition=True
    #     )
    # )
    # if response:
    #     print("status :", response.status)

    # # Update Main Image
    # response = deepstream_manager.UpdateMainImage(
    #     deepstream_message.CameraID(camera_id="test")
    # )
    # if response:
    #     print("status :", response.status)

    # # Update ROI
    # response = deepstream_manager.UpdateROI(
    #     deepstream_message.ROI(camera_id="test", polygon=[1, 2, 3, 4])
    # )
    # if response:
    #     print("status :", response.status)

    # # Get Main Image
    # response = deepstream_manager.GetMainImage(
    #     deepstream_message.CameraID(camera_id="test")
    # )
    # if response:
    #     print("image :", response.image_path)

    # # GetCameraStatus
    # response = deepstream_manager.GetCameraStatus(
    #     deepstream_message.CameraID(camera_id="test")
    # )
    # if response:
    #     print("camera_id :", response.camera_id)
    #     print("active :", response.active)
    #     print("fps :", response.fps)

    # GetCameraStatusList
    response = deepstream_manager.GetCameraStatusList(deepstream_message.Empty())
    if response:
        for item in response.camera_status_list:
            print("camera_id :", item.camera_id)
            print("active :", item.active)
            print("fps :", item.fps)
