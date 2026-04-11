import json
import random

new_artists_data = {
    "Taylor Swift": {
        "genre": "Pop",
        "albums": {
            "The Tortured Poets Department": ["Fortnight (feat Post Malone)"],
            "Folklore": ["Exile (feat Bon Iver)", "Cardigan"],
            "1989": ["Blank Space", "Bad Blood (feat Kendrick Lamar)"]
        }
    },
    "Kendrick Lamar": {
        "genre": "Hip Hop",
        "albums": {
            "DAMN": ["HUMBLE", "DNA", "LOYALTY (feat Rihanna)"],
            "Black Panther": ["All The Stars (with SZA)"]
        }
    },
    "Lana Del Rey": {
        "genre": "Alternative",
        "albums": {
            "Lust for Life": ["Lust for Life (feat The Weeknd)", "Cherry"],
            "Born to Die": ["Summertime Sadness", "Video Games"]
        }
    },
    "Miyagi and Andy Panda": {
        "genre": "Hip Hop",
        "albums": {
            "Yamakasi": ["Yamakasi", "Minor", "Captain"],
            "Hajime": ["I Got Love (feat Rem Digga)"]
        }
    },
    "Scorpions": {
        "genre": "Classic Rock",
        "albums": {
            "Crazy World": ["Wind of Change", "Send Me an Angel"],
            "Love at First Sting": ["Rock You Like a Hurricane", "Still Loving You"]
        }
    },
    "Post Malone": {
        "genre": "Pop Rap",
        "albums": {
            "Hollywoods Bleeding": ["Circles", "Sunflower (with Swae Lee)", "Take What You Want (feat Ozzy Osbourne)"],
            "Beerbongs and Bentleys": ["Rockstar (feat 21 Savage)"]
        }
    },
    "Jah Khalib": {
        "genre": "Hip Hop",
        "albums": {
            "EGO": ["Medina"],
            "If Thaaat": ["Leila", "Docha"]
        }
    },
    "Bryan Adams": {
        "genre": "Rock",
        "albums": {
            "Reckless": ["Summer of 69", "Its Only Love (with Tina Turner)"],
            "Waking Up the Neighbours": ["Everything I Do"]
        }
    },
    
    "5 Seconds of Summer": {
        "genre": "Pop Rock",
        "albums": {
            "Youngblood": ["Youngblood", "Ghost of You"],
            "World War Joy": ["Who Do You Love (with The Chainsmokers)"]
        }
    },
    "Aerosmith": {
        "genre": "Rock",
        "albums": {
            "Toys in the Attic": ["Walk This Way", "Sweet Emotion"],
            "Raising Hell": ["Walk This Way (with Run DMC)"],
            "Armageddon": ["I Dont Want to Miss a Thing"]
        }
    }
}

songs_list = []

for artist, info in new_artists_data.items():
    for album, tracks in info["albums"].items():
        for track in tracks:
            
            safe_track = track.lower().replace(" ", "_").replace("?", "").replace("'", "").replace("(", "").replace(")", "")
            safe_artist = artist.lower().replace(" ", "_").replace("5", "five_")
            safe_album = album.lower().replace(" ", "_").replace("'", "")
            
            songs_list.append({
                "track_title": track,
                "artist_name": artist,
                "genre": info["genre"],
                "album_title": album,
                "artist_image": f"media/artists_image/{safe_artist}.jpg",
                "cover_url": f"media/cover/{safe_album}.jpg",
                "file_url": f"media/songs/{safe_track}.mp3",
                "duration": random.randint(180, 280) 
            })

with open("data.json", "w") as f:
    json.dump(songs_list, f, indent=4)

print(f"Successfully generated {len(songs_list)} new songs with duets in songs_data.json!")
