from fastapi import FastAPI, Depends
from starlette.middleware.cors import CORSMiddleware
from auth import router as auth_router
from src import app_configs, settings
from typing import Dict, Any
from auth.models import User
from auth.auth import get_current_user
from src.models import Face, CommonResponse, Camera
import main
import uvicorn


app = FastAPI(**app_configs)
app.add_middleware(
    CORSMiddleware,
    allow_origins=settings.CORS_ORIGINS,
    allow_credentials=True,
    allow_methods=("GET", "POST", "PUT", "PATCH", "DELETE", "OPTIONS"),
    allow_headers=settings.CORS_HEADERS,
)
app.include_router(auth_router, tags=["auth"])


@app.get(
    "/healthcheck",
    include_in_schema=True,
    tags=["healthcheck"],
)
async def healthcheck():
    return {"status": "ok", "code": 200, "message": "Healthcheck", "error": False}


@app.get(
    "/devices", include_in_schema=True, response_model=Dict[str, Any], tags=["device"]
)
async def device(current_user: User = Depends(get_current_user)) -> Dict[str, Any]:
    status, message, cameras = main.get_cameras()
    print("cameras:", cameras)
    return {
        "status": status,
        "device_id": settings.DEVICE_ID,
        "device_name": settings.DEVICE_NAME,
        "cameras": cameras,
        "code": 200,
        "message": message,
        "error": False,
    }


@app.post(
    "/camera",
    include_in_schema=True,
    response_model=CommonResponse,
    tags=["device", "camera"],
)
async def add_camera(camera: Camera):
    status = main.add_camera(camera.name, camera.url)
    if status:
        message = "Camera added successfully"
    else:
        message = "Camera name or camera url already exists"
    return {
        "status": status,
        "message": message,
        "code": 200,
        "error": False,
    }


@app.delete(
    "/camera/{camera_id}",
    include_in_schema=True,
    response_model=CommonResponse,
    tags=["device", "camera"],
)
async def remove_camera(
    camera_id: str,
):
    status = main.remove_camera(camera_id)
    if status:
        message = "Camera removed successfully"
    else:
        message = "Camera name does not exist"
    return {
        "status": status,
        "message": message,
        "code": 200,
        "error": False,
    }


@app.get(
    "/camera",
    include_in_schema=True,
    response_model=CommonResponse,
    tags=["device", "camera"],
)
async def camera():
    status, message, cameras = main.get_cameras()
    print("cameras:", cameras)
    return {
        "status": status,
        "data": {"cameras": cameras},
        "message": message,
        "code": 200,
        "error": False,
    }


@app.get("/status", include_in_schema=True, response_model=CommonResponse)
async def status():
    status_1, message1, cameras = main.get_cameras()
    status_2, message2, face_ids = main.get_face_ids()
    message = message1 + " " + message2

    return {
        "status": status_1 and status_2,
        "data": {"cameras": cameras, "face_ids": face_ids},
        "message": message,
        "code": 200,
        "error": False,
    }


@app.get("/faces", include_in_schema=True, response_model=CommonResponse)
async def faces(current_user: User = Depends(get_current_user)):
    data = main.get_faces()
    return {
        "status": True,
        "message": "Face information",
        "data": {"faces": data},
        "code": 200,
        "error": False,
    }


@app.post("/faces", include_in_schema=True, response_model=CommonResponse)
async def faces(face: Face, current_user: User = Depends(get_current_user)):
    status, massage = main.add_faces(face)
    return {
        "status": status,
        "message": massage,
        "code": 200 if status else 400,
        "error": False,
    }


@app.get("/faces/{faceId}", include_in_schema=True, response_model=CommonResponse)
async def face(faceId: str, current_user: User = Depends(get_current_user)):
    data = main.get_face(faceId)
    if data is not None:
        return {
            "status": True,
            "message": "Face information",
            "data": data,
            "code": 200,
            "error": False,
        }
    else:
        return {
            "status": False,
            "message": "Face not found",
            "code": 404,
            "error": True,
        }


@app.delete("/faces/{faceId}", include_in_schema=True, response_model=CommonResponse)
async def get_face(faceId: str, current_user: User = Depends(get_current_user)):
    status, massage = main.remove_face(faceId)
    return {
        "status": status,
        "message": massage,
        "code": 200,
        "error": False,
    }


@app.put("/faces", include_in_schema=True, response_model=CommonResponse)
async def updateface(face: Face, current_user: User = Depends(get_current_user)):
    status, massage = main.update_face(face)
    return {
        "status": status,
        "message": massage,
        "code": 200,
        "error": False,
    }


if __name__ == "__main__":
    uvicorn.run(app, host="0.0.0.0", port=8000)
