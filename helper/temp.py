import os
import psycopg2
from mutagen import File # 'File' automatically detects MP3, WAV, FLAC, etc.

conn = psycopg2.connect(dbname="spotify_db", user="postgres", password="7711", host="localhost")
cursor = conn.cursor()

cursor.execute("SELECT id, file_url FROM tracks")
tracks = cursor.fetchall()

# Update this to point to the folder containing your music!
BASE_DIR = "/home/hnin/SpotifyClone/SpotifyBackend/" 

for track_id, file_url in tracks:
    # Strip any leading slashes from the DB url so it joins correctly
    clean_url = file_url.lstrip('/')
    full_path = os.path.join(BASE_DIR, clean_url)

    # DIAGNOSTIC CHECK: Does Linux see a file at this exact string?
    if not os.path.exists(full_path):
        print(f"❌ ERROR: Linux cannot find a file at this exact path: {full_path}")
        continue # Skip to the next song

    try:
        # Read the real audio file from the hard drive
        audio = File(full_path)
        
        if audio is None:
            print(f"⚠️ ERROR: Mutagen found the file, but couldn't read the audio data: {full_path}")
            continue

        real_duration = int(audio.info.length)

        # Update PostgreSQL
        cursor.execute("UPDATE tracks SET duration = %s WHERE id = %s", (real_duration, track_id))
        print(f"✅ Updated {full_path} -> {real_duration} seconds")
        
    except Exception as e:
        print(f"❌ Crash while processing {file_url}: {e}")

conn.commit()
conn.close()
print("\nDone checking all song durations!")