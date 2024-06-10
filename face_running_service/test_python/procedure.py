from kafka import KafkaProducer
import json


class MessageProducer:
    broker = ""
    topic = ""
    producer = None

    def __init__(self, broker, topic):
        self.broker = broker
        self.topic = topic
        self.producer = KafkaProducer(
            bootstrap_servers=self.broker,
            value_serializer=lambda v: json.dumps(v).encode("utf-8"),
            acks="all",
            retries=3,
        )

    def send_msg(self, msg):
        print("sending message...")
        try:
            future = self.producer.send(self.topic, msg)
            self.producer.flush()
            future.get(timeout=1)
            print("message sent successfully...")
            return {"status_code": 200, "error": None}
        except Exception as ex:
            return ex


broker = "127.0.0.1:9092"
topic = "Face"
message_producer = MessageProducer(broker, topic)

data = {"name": "abc", "email": "abc@example.com"}
resp = message_producer.send_msg(data)
print(resp)

data = {"name": "abc", "email": "abc@example.com"}
resp = message_producer.send_msg(data)
print(resp)
