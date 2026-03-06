CREATE TABLE artists (
    artist_id SERIAL PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    bio TEXT,
    image_url TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE albums (
    album_id SERIAL PRIMARY KEY,
    title VARCHAR(150) NOT NULL,
    artist_id INT REFERENCES artists(artist_id) ON DELETE CASCADE,
    release_date DATE,
    cover_art_url TEXT
);

CREATE TABLE songs (
    song_id SERIAL PRIMARY KEY,
    title VARCHAR(150) NOT NULL,
    album_id INT REFERENCES albums(album_id) ON DELETE CASCADE,
    artist_id INT REFERENCES artists(artist_id) ON DELETE CASCADE,
    duration_seconds INT NOT NULL,
    file_url TEXT NOT NULL,
    play_count INT DEFAULT 0
);

CREATE TABLE users (
    user_id SERIAL PRIMARY KEY,
    username VARCHAR(50) UNIQUE NOT NULL,
    email VARCHAR(100) UNIQUE NOT NULL,
    password_hash TEXT NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE user_likes (
    user_id INT REFERENCES users(user_id) ON DELETE CASCADE,
    song_id INT REFERENCES songs(song_id) ON DELETE CASCADE,
    liked_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (user_id, song_id)
);

CREATE TABLE playlists (
    playlist_id SERIAL PRIMARY KEY,
    user_id INT REFERENCES users(user_id) ON DELETE CASCADE,
    name VARCHAR(100) NOT NULL,
    is_public BOOLEAN DEFAULT TRUE
);

CREATE TABLE playlist_songs (
    playlist_id INT REFERENCES playlists(playlist_id) ON DELETE CASCADE,
    song_id INT REFERENCES songs(song_id) ON DELETE CASCADE,
    added_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (playlist_id, song_id)
);

CREATE INDEX idx_song_title ON songs(title);
