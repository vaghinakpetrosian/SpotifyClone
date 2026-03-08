# Full-Stack Spotify Clone

A high-performance desktop media player built with a **C++/Qt** frontend, a **Python/FastAPI** REST backend, and a **PostgreSQL** relational database.

---

## Overview
This project is a 3-tier desktop application designed to replicate the core user experience of Spotify. It features a responsive UI, asynchronous data fetching, and a robust backend architecture to manage user libraries and track metadata.

## Tech Stack
* **Frontend:** C++20, Qt 6 (Widgets, Network, Multimedia)
* **Backend:** Python 3.10+, FastAPI, SQLAlchemy (ORM)
* **Database:** PostgreSQL 15+
* **Styling:** Custom Qt Style Sheets (QSS) for a modern Dark Mode aesthetic

## Key Features
* **Dynamic Media Playback:** Integrated `QMediaPlayer` for seamless audio streaming with custom shuffle and loop algorithms.
* **Asynchronous API Integration:** Uses `QNetworkAccessManager` to fetch data from the FastAPI backend without freezing the UI thread.
* **Relational Data Management:** PostgreSQL database stores user profiles, liked songs, and artist relationships.
* **Real-time Search:** Optimized SQL queries for fast metadata retrieval and filtering.
* **Modern UI:** Responsive layouts with custom-styled buttons, sliders, and album art rendering.

## System Architecture
The application follows a standard Client-Server architecture:
1.  **Client (C++/Qt):** Handles user input, audio output, and UI state management.
2.  **API (FastAPI):** Acts as the bridge, providing RESTful endpoints for the frontend to consume.
3.  **Database (PostgreSQL):** Persistent storage for all relational data and user libraries.

## Project Structure
```text
├── src/                # C++ Source files (.cpp, .h)
├── ui/                 # Qt Designer files (.ui) and Resources (.qrc)
├── backend/            # FastAPI server and Python scripts
├── database/           # SQL schema and migration scripts
├── screenshots/        # Application UI previews
└── README.md
