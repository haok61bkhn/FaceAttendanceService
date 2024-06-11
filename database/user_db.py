import os
import sqlite3
from sqlite3 import Error
from typing import List, Dict, Any


class UserDB:
    def __init__(self, db_file: str):
        self.data_map = {}
        self.db_file = db_file
        self.init()

    def init(self):
        self.conn = self.create_connection()
        self.create_table()
        users = self.get_users()
        for user in users:
            self.data_map[user[8]] = user

    def create_connection(self):
        try:
            conn = sqlite3.connect(self.db_file)
            return conn
        except Error as e:
            print(e)

    def create_table(self):
        query = """
            CREATE TABLE IF NOT EXISTS UserDB (
                faceId STRING PRIMARY KEY,
                name STRING NOT NULL,
                image1 STRING NOT NULL,
                image2 STRING,
                image3 STRING,
                image4 STRING
            );
            
        """

        try:
            cursor = self.conn.cursor()
            cursor.execute(query)
        except Error as e:
            print(e)

    def add_user(self, data: Dict[str, Any]):
        query = """
            INSERT
            INTO UserDB (faceId, name, image1, image2, image3, image4)
            VALUES (?, ?, ?, ?, ?, ?);
        """
        try:
            cursor = self.conn.cursor()
            cursor.execute(query, (data))
            self.conn.commit()
            self.data_map[data[0]] = data
            return True, "Add user successfully"
        except Error as e:
            return False, str(e)

    def remove_user(self, faceId: str):
        query = """
            DELETE FROM UserDB WHERE faceId = ?;
        """
        try:
            cursor = self.conn.cursor()
            cursor.execute(query, (faceId,))
            self.conn.commit()
            del self.data_map[faceId]
            return True
        except Error as e:
            print(e)
            return False

    def get_user(self, faceId: str) -> Dict[str, Any]:
        query = """
            SELECT * FROM UserDB WHERE faceId = ?;
        """
        try:
            cursor = self.conn.cursor()
            cursor.execute(query, (faceId,))
            row = cursor.fetchone()
            return row
        except Error as e:
            print(e)
            return None

    def get_users(self) -> List[Dict[str, Any]]:
        query = """
            SELECT * FROM UserDB;
        """
        try:
            cursor = self.conn.cursor()
            cursor.execute(query)
            rows = cursor.fetchall()
            return rows
        except Error as e:
            print(e)
            return None
