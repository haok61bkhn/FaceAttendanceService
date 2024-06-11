from pydantic import BaseModel


class FaceItem(BaseModel):
    name: str
    image: str
    score: float
    timestamp: int
    camera_name: int
