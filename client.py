# client.py
import requests


def get_token(username: str, password: str) -> str:
    response = requests.post(
        "http://127.0.0.1:8000/login",
        data={"username": username, "password": password},
        headers={"Content-Type": "application/x-www-form-urlencoded"},
    )
    print(response.json())
    if response.status_code == 200:
        return response.json().get("data").get("token")
    else:
        raise Exception("Could not get token")


def get_user_info(token: str):
    response = requests.get(
        "http://127.0.0.1:8000/users/me/", headers={"Authorization": f"Bearer {token}"}
    )
    if response.status_code == 200:
        return response.json()
    else:
        raise Exception("Could not fetch user info")


def add_user(token: str):
    data = {
        "faceId": "abvc",
        "name": "John Doe",
        "image1": "",
        "image2": "",
        "image3": "",
    }
    url = "http://127.0.0.1:8000/faces"
    response = requests.post(
        url, headers={"Authorization": f"Bearer {token}"}, json=data
    )
    if response.status_code == 200:
        print(response.json())
        return response.json()
    else:
        raise Exception("Could not add user")


if __name__ == "__main__":
    username = "device_id_01"
    password = "device_01"
    token = get_token(username, password)
    print(f"Access Token: {token}")

    # user_info = get_user_info(token)
    # print(f"User Info: {user_info}")

    add_user(token)
