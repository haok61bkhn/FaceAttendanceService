from src.config import settings
from src.tools import create_folder, base64_to_cv2, cv2_to_base64
from src.consumer import MongoConsumer
from database.user_db import UserDB
from src.models import Face
import os
import cv2
from src.deepstream_manager_service import DeepStreamManagerService
import threading


file_current_dir = os.path.dirname(os.path.abspath(__file__))
image_dir = os.path.join(file_current_dir, settings.DATA_DIR, "APP_IMAGES")
database_dir = os.path.join(file_current_dir, settings.DATA_DIR, "APP_DATABASE")
create_folder(settings.DATA_DIR)
create_folder(image_dir)
create_folder(database_dir)

user_db = UserDB(os.path.join(database_dir, "user.db"))
consumer = MongoConsumer(
    user_db,
    settings.MONGO_URL,
    settings.MONGO_DB,
    settings.MONGO_COLLECTION,
    settings.DEVICE_ID,
    settings.DEVICE_NAME,
    settings.HOOK_URL,
)
thread = threading.Thread(target=consumer.run)
thread.daemon = True
thread.start()


deepstream_manager = DeepStreamManagerService(
    settings.DEEPSTREAM_MANAGER_HOST,
    settings.DEEPSTREAM_MANAGER_PORT,
    settings.DEEPSTREAM_STUB_NAME,
)


def add_camera(camera_name: str, camera_url: str):
    cameras = deepstream_manager.get_cameras()
    if cameras is None:
        return False, "Get cameras failed"
    else:
        camera_ids = {camera["camera_id"] for camera in cameras}
        camera_id = f"ch_0"
        for i in range(len(cameras) + 1):
            camera_id = f"ch_{i}"
            if camera_id not in camera_ids:
                break
        return deepstream_manager.insert_camera(camera_id, camera_name, camera_url)


def remove_camera(camera_id):
    return deepstream_manager.remove_camera(camera_id)


def get_cameras():
    data = deepstream_manager.get_cameras()
    if data is None:
        return False, "Get cameras failed", []
    else:
        return True, "Get cameras successfully", data


def get_face_ids():
    face_ids = deepstream_manager.get_face_ids()
    if face_ids is None:
        return False, "Get face ids failed", []
    else:
        return True, "Get face ids successfully", face_ids


def add_faces(data: Face):
    try:
        name = data.name
        faceId = data.faceId

        image_paths = []
        if data.image1:
            image = base64_to_cv2(data.image1)
            if image is None:
                return False, "Image1 is not valid"
            else:
                image_path = os.path.join(image_dir, f"{data.faceId}.jpg")
                cv2.imwrite(image_path, image)
                image_paths.append(image_path)
        else:
            return False, "Image1 is required"
        if data.image2:
            image = base64_to_cv2(data.image2)
            if image is None:
                return False, "Image2 is not valid"
            else:
                image_path = os.path.join(image_dir, f"{data.faceId}_1.jpg")
                cv2.imwrite(image_path, image)
                image_paths.append(image_path)
        if data.image3:
            image = base64_to_cv2(data.image3)
            if image is None:
                return False, "Image3 is not valid"
            else:
                image_path = os.path.join(image_dir, f"{data.faceId}_2.jpg")
                cv2.imwrite(image_path, image)
                image_paths.append(image_path)
        if data.image4:
            image = base64_to_cv2(data.image4)
            if image is None:
                return False, "Image4 is not valid"
            else:
                image_path = os.path.join(image_dir, f"{data.faceId}_3.jpg")
                cv2.imwrite(image_path, image)
                image_paths.append(image_path)

        status, face_crop_paths, message = deepstream_manager.insert_face(
            faceId, image_paths
        )

        if status:
            if len(face_crop_paths) < 4:
                for i in range(4 - len(face_crop_paths)):
                    face_crop_paths.append(None)
            data = [
                faceId,
                name,
                face_crop_paths[0],
                face_crop_paths[1],
                face_crop_paths[2],
                face_crop_paths[3],
            ]

            status, message = user_db.add_user(data)
            if not status:
                return False, message

            return True, "Add face successfully"
        else:
            return False, message
    except Exception as e:
        print(e)
        return False, str(e)


def get_faces():
    data = user_db.get_users()
    print(data)
    if data is None:
        return []
    else:
        faces = []
        for row in data:
            face = {
                "faceId": row[0],
                "name": row[1],
                "image1": "",
                "image2": "",
                "image3": "",
                "image4": "",
            }
            image_path_1 = row[2]
            image_1 = cv2.imread(image_path_1)
            if image_1 is not None:
                face["image1"] = cv2_to_base64(image_1)
            image_path_2 = row[3]
            if image_path_2 is not None:
                image_2 = cv2.imread(image_path_2)
                if image_2 is not None:
                    face["image2"] = cv2_to_base64(image_2)
            image_path_3 = row[4]
            if image_path_3 is not None:
                image_3 = cv2.imread(image_path_3)
                if image_3 is not None:
                    face["image3"] = cv2_to_base64(image_3)
            image_path_4 = row[5]
            if image_path_4 is not None:
                image_4 = cv2.imread(image_path_4)
                if image_4 is not None:
                    face["image4"] = cv2_to_base64(image_4)
            faces.append(face)
        return faces


def get_face(faceId: str):
    data = user_db.get_user(faceId)
    if data is None:
        return None
    else:
        face = {
            "faceId": data[0],
            "name": data[1],
            "image1": "",
            "image2": "",
            "image3": "",
            "image4": "",
        }
        image_path_1 = data[2]

        image_1 = cv2.imread(image_path_1)
        if image_1 is not None:
            face["image1"] = cv2_to_base64(image_1)
        image_path_2 = data[3]
        if image_path_2 is not None:
            image_2 = cv2.imread(image_path_2)
            if image_2 is not None:
                face["image2"] = cv2_to_base64(image_2)
        image_path_3 = data[4]
        if image_path_3 is not None:
            image_3 = cv2.imread(image_path_3)
            if image_3 is not None:
                face["image3"] = cv2_to_base64(image_3)
        image_path_4 = data[5]
        if image_path_4 is not None:
            image_4 = cv2.imread(image_path_4)
            if image_4 is not None:
                face["image4"] = cv2_to_base64(image_4)

        return face


def remove_face(faceId: str):
    status = user_db.remove_user(faceId)
    if not status:
        return False, "Remove face failed"
    status = deepstream_manager.remove_face(faceId)
    if not status:
        return False, "Remove feature failed"
    return True, "Remove face successfully"


def update_face(data: Face):
    faceId = data.faceId
    status, message = remove_face(faceId)
    if not status:
        return False, message
    status, message = add_faces(data)
    return status, message
