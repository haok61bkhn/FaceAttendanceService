from fastapi import Depends, HTTPException, status
from fastapi.security import OAuth2PasswordBearer
from src.config import settings
from jose import JWTError, jwt
from .secret import DEVICE_HASHED_PASSWORD

from .models import TokenData, User, UserInDB
from .utils import (
    verify_password,
    SECRET_KEY,
    ALGORITHM,
)

oauth2_scheme = OAuth2PasswordBearer(tokenUrl="login")

user_db = {
    settings.DEVICE_ID: {
        "user_name": settings.DEVICE_ID,
        "device_name": settings.DEVICE_NAME,
        "hashed_password": DEVICE_HASHED_PASSWORD,
    }
}


def get_user(db, username: str):
    if username in db:
        user_dict = db[username]
        return UserInDB(**user_dict)


def authenticate_user(user_db, username: str, password: str):
    user = get_user(user_db, username)
    if not user:
        return False
    if not verify_password(password, user.hashed_password):
        return False
    return user


async def get_current_user(token: str = Depends(oauth2_scheme)):
    credentials_exception = HTTPException(
        status_code=status.HTTP_401_UNAUTHORIZED,
        detail="Could not validate credentials",
        headers={"WWW-Authenticate": "Bearer"},
    )
    try:
        payload = jwt.decode(token, SECRET_KEY, algorithms=[ALGORITHM])
        username: str = payload.get("sub")
        if username is None:
            raise credentials_exception
        token_data = TokenData(user_name=username)
    except JWTError:
        raise credentials_exception
    user = get_user(user_db, username=token_data.user_name)
    if user is None:
        raise credentials_exception
    return user


async def get_current_active_user(current_user: User = Depends(get_current_user)):
    return current_user
