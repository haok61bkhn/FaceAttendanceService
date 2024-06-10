from typing import Any, Optional, List, Dict
from pydantic_settings import BaseSettings
from pydantic import Field
from .constants import Environment


class Config(BaseSettings):
    DEVICE_ID: str = Field("device_01", env="DEVICE_ID")
    DEVICE_NAME: str = Field("Device 01", env="DEVICE_NAME")
    SITE_DOMAIN: str = Field("myapp.com", env="SITE_DOMAIN")

    ENVIRONMENT: Environment = Field(Environment.PRODUCTION, env="ENVIRONMENT")
    CORS_ORIGINS: str = Field("*", env="CORS_ORIGINS")
    CORS_HEADERS: str = Field("*", env="CORS_HEADERS")
    DATA_DIR: str = Field("./data", env="DATA_DIR")
    CAMERA_DB: str = Field("camera.db", env="CAMERA_DB")
    APP_VERSION: str = Field("1", env="APP_VERSION")

    DEEPSTREAM_MANAGER_HOST: str = Field("localhost", env="DEEPSTREAM_MANAGER_HOST")
    DEEPSTREAM_MANAGER_PORT: int = Field(50051, env="DEEPSTREAM_MANAGER_PORT")
    DEEPSTREAM_STUB_NAME: str = Field("DeepStreamManager", env="DEEPSTREAM_STUB_NAME")
    MONGO_URL: str = Field("mongodb://localhost:27017", env="MONGO_URL")
    MONGO_DB: str = Field("mydb", env="MONGO_DB")
    MONGO_COLLECTION: str = Field("mycollection", env="MONGO_COLLECTION")
    HOOK_URL: str = Field("http://localhost:8000/hook", env="HOOK_URL")

    class Config:
        env_file = ".env"
        env_file_encoding = "utf-8"


settings = Config()
app_configs: Dict[str, Any] = {"title": "App API"}
if settings.ENVIRONMENT.is_deployed:
    app_configs["root_path"] = f"/v{settings.APP_VERSION}"
if not settings.ENVIRONMENT.is_debug:
    app_configs["openapi_url"] = None
