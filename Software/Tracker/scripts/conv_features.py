# converts features from [0..1] to [-1..1]
import struct
import sqlite3


conn = sqlite3.connect("train.db")
c = conn.cursor()

c.execute("select count(*) from candidates")

count = c.fetchone()[0]

converted_rows = []
for row in c.execute("select hash, features from candidates"):
    h = row[0]
    feature_blob = row[1]
    features = struct.unpack("<3780f", feature_blob)
    remapped = map(lambda f: f * 2.0 - 1.0, features)
    converted_rows.append((h, remapped))

for row in converted_rows:
    h = row[0]
    features = row[1]
    blob = buffer(struct.pack("%sf" % len(features), *features))
    c.execute("update candidates set features = ? where hash = ?", [blob, h])

conn.commit()
conn.close()
