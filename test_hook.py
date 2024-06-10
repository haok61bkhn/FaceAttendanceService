import requests
import json

url = "https://qbs-api.baevnai.com/v1/faces/machine-hook"

payload = {
    "faceInfo": {
        "sex": 0,
        "age": 27,
        "image1": "stringBase64",
        "image2": "stringBase64",
        "image3": None,
        "feature": "",
        "name": "Lê Văn Trung",
        "schoolId": "44450450",
        "classId": "11223201",
        "studentId": "1122320101",
        "identityCard": "",
        "job": "Software",
        "phone": "0353814306",
        "email": "levantrungeale28595@gmail.com",
        "note": "123123.",
    },
    "deviceInfo": {
        "id": "democongty",
        "name": "democongty",
        "camera_name": "camera_1",
        "camera_id": "ch1",
        "time": 1717340079,
    },
}
headers = {"Content-Type": "application/json"}

response = requests.post(url, headers=headers, data=json.dumps(payload))

if response.status_code == 200:
    print("Request was successful.")
    print("Response:", response.json())
else:
    print("Request failed.")
    print("Status Code:", response.status_code)
    print("Response:", response.text)
