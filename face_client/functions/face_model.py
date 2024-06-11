from pydantic import BaseModel


class FaceItem(BaseModel):
    name: str
    image: str
    score: float
