# auth/models.py
from pydantic import BaseModel
from typing import Union, Optional


class Token(BaseModel):
    code: int
    message: str
    error: bool
    data: dict
    access_token: str
    token_type: str


class TokenData(BaseModel):
    user_name: Union[str, None] = None


class User(BaseModel):
    user_name: str
    device_name: Union[str, None] = None


class UserInDB(User):
    hashed_password: str
