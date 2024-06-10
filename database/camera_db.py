import os
import sqlite3
from sqlite3 import Error
from typing import List, Dict, Any


class CameraDB:
    def __init__(self, db_file: str):
        self.db_file = db_file
        self.conn = self.create_connection()
        self.create_table()

    def create_connection(self):
        try:
            conn = sqlite3.connect(self.db_file)
            return conn
        except Error as e:
            print(e)

    def create_table(self):
        query = """
        CREATE TABLE IF NOT EXISTS camera (
            camera_id INTEGER PRIMARY KEY AUTOINCREMENT,
            camera_name TEXT NOT NULL,
            camera_url TEXT NOT NULL
        );
        """
        try:
            cursor = self.conn.cursor()
            cursor.execute(query)
        except Error as e:
            print(e)

    def add_camera(self, camera_name: str, camera_url: str):
        query = """
        INSERT INTO camera (camera_name, camera_url) VALUES (?, ?);
        """
        try:
            cursor = self.conn.cursor()
            cursor.execute(query, (camera_name, camera_url))
            self.conn.commit()
            return True
        except Error as e:
            print(e)
            return False

    def get_cameras(self) -> List[Dict[str, Any]]:
        query = """
        SELECT * FROM camera;
        """
        try:
            cursor = self.conn.cursor()
            cursor.execute(query)
            rows = cursor.fetchall()
            return [
                {
                    "camera_id": "ch" + str(row[0]),
                    "camera_name": row[1],
                    "camera_url": row[2],
                }
                for row in rows
            ]
        except Error as e:
            print(e)

    def get_camera(self, camera_id: str) -> Dict[str, Any]:
        query = """
        SELECT * FROM camera WHERE camera_id = ?;
        """
        try:
            camera_id = camera_id[2:]
            cursor = self.conn.cursor()
            cursor.execute(query, (camera_id,))
            row = cursor.fetchone()
            return None if not row else {"camera_name": row[1], "camera_url": row[2]}
        except Error as e:
            print(e)
            return None

    def remove_camera(self, camera_id: str):
        camera_id = camera_id[2:]
        query = """
        DELETE FROM camera WHERE camera_id = ?;
        """
        try:
            cursor = self.conn.cursor()
            cursor.execute(query, (camera_id,))
            self.conn.commit()
            return True
        except Error as e:
            print(e)
            return False


# Path: database/camera_db.py
