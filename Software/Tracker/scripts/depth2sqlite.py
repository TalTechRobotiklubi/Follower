#!/usr/bin/env python
import argparse
import sqlite3
import re

def atoi(text):
    return int(text) if text.isdigit() else text

def n_key(text):
    return [atoi(c) for c in re.split("(\d+)", text)]

parser = argparse.ArgumentParser("Adds input depth files to an SQLite DB")
parser.add_argument("-f", "--files", nargs="+", type=str)
parser.add_argument("-d", "--database", default="depth.db")

args = parser.parse_args()

args.files.sort(key=n_key)

conn = sqlite3.connect(args.database)
c = conn.cursor()
c.execute("CREATE TABLE IF NOT EXISTS depth_frames (data BLOB);")
for depth_file in args.files:
    with open(depth_file, "rb") as f:
        content = f.read()
        c.execute("INSERT INTO depth_frames VALUES (?)", [buffer(content)])

conn.commit()
conn.close()

print "done"
