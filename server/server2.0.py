from flask import Flask, request, jsonify
import socket
import requests  # Import requests for API calls
import os  # ✅ Import os for environment variables
from dotenv import load_dotenv

load_dotenv()

app = Flask(__name__)

GOOGLE_API_KEY = os.getenv("GOOGLE_MAP_API_KEY")  # ✅ Get API key from environment variable

# Function to get the local IP address
def get_ip_address():
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.connect(("8.8.8.8", 80))
        ip = s.getsockname()[0]
        s.close()
        return ip
    except:
        return "127.0.0.1"

# Function to get address from coordinates using Google Maps API
def get_address_from_coordinates(lat, lon):
    if not GOOGLE_API_KEY:
        print("[ERROR] Missing Google API Key! Set it as an environment variable.")
        return "API Key not set"

    url = f"https://maps.googleapis.com/maps/api/geocode/json?latlng={lat},{lon}&key={GOOGLE_API_KEY}"
    
    try:
        response = requests.get(url)
        data = response.json()

        if data["status"] == "OK":
            address = data["results"][0]["formatted_address"]
            return address
        else:
            return "Address not found"
    except Exception as e:
        print(f"[ERROR] Google API Request Failed: {e}")
        return "Error fetching address"

@app.route("/get_address", methods=["POST"])
def get_coordinates():
    data = request.json
    latitude = data.get("latitude")
    longitude = data.get("longitude")

    if latitude is None or longitude is None:
        print("[ERROR] Invalid data received!")
        return jsonify({"error": "Invalid data"}), 400

    print(f"[INFO] Received Coordinates: Latitude = {latitude}, Longitude = {longitude}")

    # Get the actual address
    address = get_address_from_coordinates(latitude, longitude)
    print(f"[INFO] Resolved Address: {address}")

    return jsonify({"message": "Coordinates received successfully!", "address": address}), 200

if __name__ == "__main__":
    server_ip = get_ip_address()
    print(f"[INFO] Flask server is running on http://{server_ip}:5000")
    app.run(host="0.0.0.0", port=5000, debug=True)
