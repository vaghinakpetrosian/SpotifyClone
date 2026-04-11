import psycopg2
import json

def get_db_connection():
    return psycopg2.connect(
        dbname="spotify_db", 
        user="postgres", 
        password="password", #!!!!!!!!!!!!
        host="localhost"
    )

def seed_database():
    conn = get_db_connection()
    cursor = conn.cursor()
    with open('data.json', 'r') as file:
        songs = json.load(file)

    print(f"Starting to insert {len(songs)} songs...")

    for song in songs:
        try:
            cursor.execute("SELECT id FROM artists WHERE name = %s", (song['artist_name'],))
            artist = cursor.fetchone()
            
            if artist:
                artist_id = artist[0]
            else:
                cursor.execute(
                    "INSERT INTO artists (name, genre, image_url) VALUES (%s, %s, %s) RETURNING id",
                    (song['artist_name'], song['genre'], song['artist_image'])
                )
                artist_id = cursor.fetchone()[0]

            cursor.execute(
                "SELECT id FROM albums WHERE title = %s AND artist_id = %s", 
                (song['album_title'], artist_id)
            )
            album = cursor.fetchone()
            
            if album:
                album_id = album[0]
            else:
                cursor.execute(
                    "INSERT INTO albums (title, artist_id, cover_url) VALUES (%s, %s, %s) RETURNING id",
                    (song['album_title'], artist_id, song['cover_url'])
                )
                album_id = cursor.fetchone()[0]

            cursor.execute(
                "SELECT id FROM tracks WHERE title = %s AND album_id = %s",
                (song['track_title'], album_id)
            )
            if not cursor.fetchone():
                cursor.execute(
                    "INSERT INTO tracks (title, album_id, artist_id, duration_seconds, file_url) VALUES (%s, %s, %s, %s, %s)",
                    (song['track_title'], album_id, artist_id, song.get('duration', 0), song['file_url'])
                )
                print(f"Inserted: {song['track_title']} by {song['artist_name']}")
            else:
                print(f"Skipped (Already exists): {song['track_title']}")

        except Exception as e:
            print(f"Error inserting {song['track_title']}: {e}")
            conn.rollback() 

    conn.commit()
    conn.close()
    print("Database seeding complete!")

if __name__ == "__main__":
    seed_database()
