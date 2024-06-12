from fastapi import FastAPI
from functions.face_model import FaceItem
from multiprocessing import Queue
import uvicorn
from main_ui import MainUi
from PyQt5 import QtWidgets
import sys
import threading
import cv2
from functions.tools import base64_to_cv2

app = FastAPI()
face_queue = Queue()


@app.post("/face_item")
def create_face_item(face_item: FaceItem):
    global face_queue
    try:
        image = base64_to_cv2(face_item.image)
        face_queue.put(
            (
                image,
                face_item.name,
                face_item.score,
                face_item.timestamp,
                face_item.camera_name,
            )
        )
    except Exception as e:
        print(e)
    return {"code": 200, "message": "Success"}


def start_fastapi():
    uvicorn.run(app, host="0.0.0.0", port=8001)


if __name__ == "__main__":
    # Start the FastAPI server in a separate thread
    fastapi_thread = threading.Thread(target=start_fastapi, daemon=True)
    fastapi_thread.start()

    # Start the PyQt application
    qt_app = QtWidgets.QApplication(sys.argv)
    window = MainUi(face_queue)
    window.show()

    sys.exit(qt_app.exec_())
