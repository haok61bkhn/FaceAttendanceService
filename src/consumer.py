from .config import settings
from .tools import cv2_to_base64
import json
import cv2
import requests
import os
import time
import pymongo


class MongoConsumer:
    def __init__(
        self,
        user_db,
        mongo_url,
        mongo_db,
        mongo_collection,
        device_id,
        device_name,
        hook_url="",
    ):
        self.user_db = user_db
        self.mongo_url = mongo_url
        self.mongo_db = mongo_db
        self.mongo_collection = mongo_collection
        self.device_id = device_id
        self.device_name = device_name
        self.hook_url = hook_url
        self.client = None
        self.db = None
        self.collection = None
        self.connect()

    def connect(self):
        self.client = pymongo.MongoClient(self.mongo_url)
        self.db = self.client[self.mongo_db]
        self.collection = self.db[self.mongo_collection]

    def set_hook_url(self, hook_url):
        self.hook_url = hook_url

    def hook_face(
        self,
        face_id,
        camera_id,
        camera_name,
        object_image_path,
        object_image_full_path,
        time_stamp,
    ):
        if self.hook_url == "":
            return True
        if face_id not in self.user_db.data_map:
            print("Face not found in database")
            return True
        face_data = self.user_db.data_map[face_id]
        object_image = cv2.imread(object_image_path)
        object_image_full = cv2.imread(object_image_full_path)
        image_1 = None
        image_2 = None
        if object_image is None or object_image_full is None:
            time.sleep(0.1)
            object_image = cv2.imread(object_image_path)
            object_image_full = cv2.imread(object_image_full_path)
        if object_image_full is not None:
            image_2 = cv2_to_base64(object_image_full)
            data = {
                "image": image_2,
                "name": face_data[1],
                "score": float(1.0),
                "timestamp": int(time_stamp),
                "camera_name": camera_name,
            }
        print("HOOK",face_id)
        try:
            response = requests.post(self.hook_url, json=data)
            print("Hook response:", response.text)
        except Exception as e:
            print("Hook error:", e)
            pass
        return True

    def run(self):
        while True:
            # reverse with sort([("time_stamp", -1)])
            cursor = self.collection.find().sort([("time_stamp", 1)])
            for document in cursor:
                print("Document",document)
                face_id = document["face_id"]
                camera_id = document["camera_id"]
                camera_name = document["camera_name"]
                object_image_path = document["object_image_path"]
                object_image_full_path = document["object_image_full_path"]
                time_stamp = document["time_stamp"]

                if self.hook_face(
                    face_id,
                    camera_id,
                    camera_name,
                    object_image_path,
                    object_image_full_path,
                    time_stamp,
                ):
                    if os.path.exists(object_image_path):
                        os.remove(object_image_path)
                    if os.path.exists(object_image_full_path):
                        os.remove(object_image_full_path)
                    self.collection.delete_one({"_id": document["_id"]})
                    print("Hook success")
            time.sleep(0.1)
