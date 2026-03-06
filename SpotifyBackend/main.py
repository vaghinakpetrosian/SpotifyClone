from fastapi import FastAPI, HTTPException
from fastapi.staticfiles import StaticFiles
import psycopg2
from psycopg2.extras import RealDictCursor
import os

app = FastAPI()

os.makedirs("media", exist_ok=True)
app.mount("/media", StaticFiles(directory="media"), name="media")

def get_db():
    return psycopg2.connect(
        dbname="spotify_db", 
        user="postgres", 
        password="7711", 
        host="localhost"
    )

@app.post("/login/{username}")
def login(username: str):
    conn = get_db()
    cursor = conn.cursor(cursor_factory=RealDictCursor)
    try:
        cursor.execute("SELECT id, username FROM users WHERE username = %s", (username,))
        user = cursor.fetchone()
        if not user:
            cursor.execute("INSERT INTO users (username) VALUES (%s) RETURNING id, username", (username,))
            user = cursor.fetchone()
            
        conn.commit()
        return {"user_id": user["id"], "username": user["username"]}
    except Exception as e:
        conn.rollback()
        raise HTTPException(status_code=500, detail=str(e))
    finally:
        conn.close()

@app.get("/search/{query}")
def search(query: str):
    conn = get_db()
    cursor = conn.cursor(cursor_factory=RealDictCursor)
    search_pattern = f"%{query}%"
    cursor.execute("""
        SELECT t.id, t.title, a.name AS artist, a.genre, t.file_url, al.cover_url 
        FROM tracks t
        JOIN albums al ON t.album_id = al.id
        JOIN artists a ON al.artist_id = a.id
        WHERE t.title ILIKE %s OR a.name ILIKE %s
        LIMIT 20
    """, (search_pattern, search_pattern))
    results = cursor.fetchall()
    conn.close()
    return {"results": results}

@app.get("/tracks")
def get_all_tracks():
    conn = get_db()
    cursor = conn.cursor(cursor_factory=RealDictCursor)
    cursor.execute("""
        SELECT t.id, t.title, a.name AS artist, a.genre, t.file_url, al.cover_url 
        FROM tracks t
        JOIN albums al ON t.album_id = al.id
        JOIN artists a ON al.artist_id = a.id
    """)
    tracks = cursor.fetchall()
    conn.close()
    return {"tracks": tracks}

@app.post("/follow/{user_id}/{artist_id}")
def toggle_follow(user_id: int, artist_id: int):
    conn = get_db()
    cursor = conn.cursor()
    try:
        cursor.execute("SELECT id FROM users WHERE id = %s", (user_id,))
        if not cursor.fetchone():
            cursor.execute("INSERT INTO users (id, username) VALUES (%s, 'demo_user_%s')", (user_id, user_id))
        
        cursor.execute("SELECT id FROM user_followed_artists WHERE user_id = %s AND artist_id = %s", (user_id, artist_id))
        if cursor.fetchone():
            cursor.execute("DELETE FROM user_followed_artists WHERE user_id = %s AND artist_id = %s", (user_id, artist_id))
            status = "unfollowed"
        else:
            cursor.execute("INSERT INTO user_followed_artists (user_id, artist_id) VALUES (%s, %s)", (user_id, artist_id))
            status = "followed"
        conn.commit()
        return {"status": status}
    except Exception as e:
        conn.rollback()
        raise HTTPException(status_code=500, detail=str(e))
    finally:
        conn.close()

@app.post("/like/{user_id}/{track_id}")
def toggle_like(user_id: int, track_id: int):
    conn = get_db()
    cursor = conn.cursor()
    try:
        cursor.execute("SELECT id FROM user_likes WHERE user_id = %s AND track_id = %s", (user_id, track_id))
        if cursor.fetchone():
            cursor.execute("DELETE FROM user_likes WHERE user_id = %s AND track_id = %s", (user_id, track_id))
            status = "unliked"
        else:
            cursor.execute("INSERT INTO user_likes (user_id, track_id) VALUES (%s, %s)", (user_id, track_id))
            status = "liked"
        conn.commit()
        return {"status": status}
    except Exception as e:
        conn.rollback()
        raise HTTPException(status_code=500, detail=str(e))
    finally:
        conn.close()

@app.get("/liked_songs/{user_id}")
def get_liked_songs(user_id: int):
    conn = get_db()
    cursor = conn.cursor(cursor_factory=RealDictCursor)
    cursor.execute("""
        SELECT t.id, t.title, a.name AS artist, a.genre, t.file_url, al.cover_url 
        FROM tracks t 
        JOIN albums al ON t.album_id = al.id
        JOIN artists a ON al.artist_id = a.id
        JOIN user_likes ul ON t.id = ul.track_id 
        WHERE ul.user_id = %s
    """, (user_id,))
    songs = cursor.fetchall()
    conn.close()
    return {"songs": songs}

@app.get("/suggest_artists/{user_id}")
def suggest_artists(user_id: int):
    conn = get_db()
    cursor = conn.cursor(cursor_factory=RealDictCursor)
    cursor.execute("""
        WITH UserGenres AS (
            SELECT a.genre FROM artists a 
            JOIN user_followed_artists ufa ON a.id = ufa.artist_id 
            WHERE ufa.user_id = %(uid)s
            UNION ALL
            SELECT a.genre FROM artists a 
            JOIN albums al ON a.id = al.artist_id 
            JOIN tracks t ON al.id = t.album_id 
            JOIN user_likes ul ON t.id = ul.track_id 
            WHERE ul.user_id = %(uid)s
        ),
        RankedGenres AS (
            SELECT genre, COUNT(*) as weight 
            FROM UserGenres GROUP BY genre
        )
        SELECT DISTINCT a.id, a.name, a.genre, a.image_url, COALESCE(rg.weight, 0) as score
        FROM artists a
        LEFT JOIN RankedGenres rg ON a.genre = rg.genre
        WHERE NOT EXISTS (
            SELECT 1 FROM user_followed_artists ufa 
            WHERE ufa.artist_id = a.id AND ufa.user_id = %(uid)s
        )
        ORDER BY score DESC, a.name ASC 
        LIMIT 20
    """, {'uid': user_id})
    artists = cursor.fetchall()
    conn.close()
    return {"artists": artists}

@app.get("/recommend/{user_id}")
def recommend_for_user(user_id: int):
    conn = get_db()
    cursor = conn.cursor(cursor_factory=RealDictCursor)
    cursor.execute("""
        WITH UserGenres AS (
            SELECT a.genre FROM artists a 
            JOIN user_followed_artists ufa ON a.id = ufa.artist_id 
            WHERE ufa.user_id = %(uid)s
            UNION ALL
            SELECT a.genre FROM artists a 
            JOIN albums al ON a.id = al.artist_id 
            JOIN tracks t ON al.id = t.album_id 
            JOIN user_likes ul ON t.id = ul.track_id 
            WHERE ul.user_id = %(uid)s
        ),
        RankedGenres AS (
            SELECT genre, COUNT(*) as weight 
            FROM UserGenres GROUP BY genre
        )
        SELECT t.id, t.title, a.name AS artist, a.genre, t.file_url, al.cover_url,
               COALESCE(rg.weight, 0) + CASE WHEN ufa.artist_id IS NOT NULL THEN 100 ELSE 0 END as score
        FROM tracks t
        JOIN albums al ON t.album_id = al.id
        JOIN artists a ON al.artist_id = a.id
        LEFT JOIN RankedGenres rg ON a.genre = rg.genre
        LEFT JOIN user_followed_artists ufa ON a.id = ufa.artist_id AND ufa.user_id = %(uid)s
        WHERE t.id NOT IN (SELECT track_id FROM user_likes WHERE user_id = %(uid)s)
        ORDER BY score DESC
        LIMIT 20
    """, {'uid': user_id})
    recommendations = cursor.fetchall()
    conn.close()
    return {"recommendations": recommendations}

@app.get("/followed_artists/{user_id}")
def get_followed_artists(user_id: int):
    conn = get_db()
    cursor = conn.cursor(cursor_factory=RealDictCursor)
    cursor.execute("""
        SELECT a.id, a.name, a.genre, a.image_url 
        FROM artists a
        JOIN user_followed_artists ufa ON a.id = ufa.artist_id
        WHERE ufa.user_id = %s
    """, (user_id,))
    artists = cursor.fetchall()
    conn.close()
    return {"artists": artists}