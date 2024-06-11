from pydantic import BaseModel
from typing import Optional


class CommonResponse(BaseModel):
    status: bool
    message: str
    code: int
    error: bool
    data: Optional[dict] = None


class FaceID(BaseModel):
    faceId: str


class Camera(BaseModel):
    name: str
    url: str


class Face(BaseModel):
    faceId: str
    name: str
    image1: str
    image2: Optional[str] = None
    image3: Optional[str] = None
    image4: Optional[str] = None


class HookInfo(BaseModel):
    url: str
