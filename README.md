# Spotify Clone (Desktop Edition) 

A high-performance music streaming desktop application built with **C++ 17** and **Qt**, featuring a **FastAPI** backend and **PostgreSQL** database. This project replicates the core Spotify experience, including real-time synchronized lyrics, user authentication, and personalized recommendations.

## Features

* **Full Window "Now Playing" Experience:** Seamlessly transitions to a distraction-free view with dynamic background coloring based on album art.
* **Synchronized Interactive Lyrics:** Real-time lyric highlighting that follows the music. Click any lyric line to jump to that specific part of the song.
* **Dynamic UI:** Fully responsive interface that matches window sizing, preventing horizontal scrolling and maintaining layout integrity.
* **Personalization:** User "Like" system for tracks and "Follow" system for artists.
* **Recommendation System:** Backend integration for "My Vibe" suggestions.
* **Advanced Audio Controls:** Support for shuffle, repeat-one, and custom N-track sequence looping.

## Tech Stack

**Frontend:**
* **C++ 17:** Core application logic and memory management.
* **Qt Framework:** Cross-platform UI development and signal/slot architecture.

**Backend:**
* **Python / FastAPI:** High-performance REST API for data retrieval.
* **PostgreSQL:** Relational database for managing users, artists, albums, and tracks.

**Tooling:**
* **Git/GitHub:** Version control and collaboration.
* **Linux/Ubuntu:** Primary development and deployment environment.

## Project Structure

```text
├── src/                # C++ Source files (.cpp)
├── include/            # C++ Header files (.h)
├── backend/            # FastAPI Python scripts
├── database/           # SQL schema and migration files
├── icons/              # UI Assets and media
└── main.cpp            # Entry point
