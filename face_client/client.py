import requests

data = {"name": "a", "image": "/home/haobk/obama.jpg", "score": 0.9}
url = "http://localhost:8001/face_item"
response = requests.post(url, json=data)
print(response.json())
