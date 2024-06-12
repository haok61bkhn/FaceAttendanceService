# write login db with ip, username, password using sqlite3
import sqlite3
from sqlite3 import Error


class LoginDB:
    def __init__(self, db_file):
        self.db_file = db_file
        self.conn = self.create_connection()
        self.create_table()

    def create_connection(self):
        conn = None
        try:
            conn = sqlite3.connect(self.db_file)
            return conn
        except Error as e:
            print(e)
        return conn

    def create_table(self):
        create_table_sql = """CREATE TABLE IF NOT EXISTS login (
                                    id integer PRIMARY KEY,
                                    ip text NOT NULL,
                                    username text NOT NULL,
                                    password text NOT NULL,
                                    hook_url text
                                );"""
        try:
            c = self.conn.cursor()
            c.execute(create_table_sql)
        except Error as e:
            print(e)

    def save_login_infor(self, ip, username, password):
        self.delete_login_infor()
        sql = """INSERT INTO login(ip, username, password) VALUES(?,?,?)"""
        cur = self.conn.cursor()
        cur.execute(sql, (ip, username, password))
        self.conn.commit()
        return cur.lastrowid

    def get_hook_url(self):
        cur = self.conn.cursor()
        cur.execute("SELECT hook_url FROM login")
        rows = cur.fetchall()
        if len(rows) == 0:
            return None
        return rows[0][0]

    def insert_hook_url(self, hook_url):
        sql = "UPDATE login SET hook_url = ?"
        cur = self.conn.cursor()
        cur.execute(sql, (hook_url,))
        self.conn.commit()

    def get_login_infor(self):
        cur = self.conn.cursor()
        cur.execute("SELECT * FROM login")
        rows = cur.fetchall()
        return rows

    def delete_login_infor(self):
        sql = "DELETE FROM login"
        cur = self.conn.cursor()
        cur.execute(sql)
        self.conn.commit()

    def close(self):
        self.conn.close()
