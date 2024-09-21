import json
import requests

response = requests.post(
    "http://192.168.1.11:11434/api/chat",
    data=json.dumps({
        "messages": [{"role": "user", "content": "Hey"}],
        "model": "Jade",
        "stream": False
    })
)
print(response.json())