from src.deepstream_manager_service import DeepStreamManagerService


if __name__ == "__main__":
    deepstreammng = DeepStreamManagerService(
        "localhost", 50051, "DeepstreamServiceStub"
    )

    ## Insert Camera
    # camera_id = "camera_test"
    # camera_name = "Camera test"
    # camera_url = "file:///home/haobk/video.mp4"
    # status = deepstreammng.insert_camera(camera_id, camera_name, camera_url)
    # print("InsertCamera:", status)

    # cameras = deepstreammng.get_cameras()
    # print(cameras)

    # ## Remove Camera
    # camera_id = "camera_test"
    # status = deepstreammng.remove_camera(camera_id)
    # print("RemoveCamera:", status)

    # cameras = deepstreammng.get_cameras()
    # print(cameras)

    # get face ids
    face_ids = deepstreammng.get_face_ids()
    print(face_ids)

    # Insert Face
    face_id = "face_test"
    face_image_paths = ["/home/haobk/obama.jpg", "/home/haobk/obama.jpg"]
    status, face_crop_paths, message = deepstreammng.insert_face(
        face_id, face_image_paths
    )
    if status:
        print("InsertFace:", status)
        print("Face crop paths:", face_crop_paths)
    else:
        print(message)

    face_id = "face_test2"

    face_image_paths = ["/home/haobk/test3.png", "/home/haobk/test3.png"]
    status, face_crop_paths, message = deepstreammng.insert_face(
        face_id, face_image_paths
    )
    if status:
        print("InsertFace:", status)
        print("Face crop paths:", face_crop_paths)
    else:
        print(message)

    # # remove face
    # face_id = "face_test"
    # status = deepstreammng.remove_face(face_id)
    # print("RemoveFace:", status)

    # # get face ids
    face_ids = deepstreammng.get_face_ids()
    print(face_ids)
