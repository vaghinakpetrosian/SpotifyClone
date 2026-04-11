import os
import psycopg2
from mutagen import File 

conn = psycopg2.connect(dbname="spotify_db", user="postgres", password="password", host="localhost")#!!!!!!!!!!!!!
cursor = conn.cursor()

cursor.execute("SELECT id, file_url FROM tracks")
tracks = cursor.fetchall()

BASE_DIR = ".../SpotifyClone/SpotifyBackend/" #!!!!!!!!!!!!!

for track_id, file_url in tracks:
    clean_url = file_url.lstrip('/')
    full_path = os.path.join(BASE_DIR, clean_url)
    if not os.path.exists(full_path):
        print(f"ERROR: Linux cannot find a file at this exact path: {full_path}")
        continue 

    try:
        audio = File(full_path)
        
        if audio is None:
            print(f"ERROR: Mutagen found the file, but couldn't read the audio data: {full_path}")
            continue

        real_duration = int(audio.info.length)

        cursor.execute("UPDATE tracks SET duration = %s WHERE id = %s", (real_duration, track_id))
        print(f"Updated {full_path} -> {real_duration} seconds")
        
    except Exception as e:
        print(f"Crash while processing {file_url}: {e}")

conn.commit()
conn.close()
print("\nDone checking all song durations!")
