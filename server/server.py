from flask import Flask, request, jsonify
import socket

app = Flask(__name__)

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

@app.route("/get_address", methods=["POST"])
def get_coordinates():
    data = request.json
    latitude = data.get("latitude")
    longitude = data.get("longitude")

    if latitude is None or longitude is None:
        print("[ERROR] Invalid data received!")
        return jsonify({"error": "Invalid data"}), 400

    print(f"[INFO] Received Coordinates: Latitude = {latitude}, Longitude = {longitude}")

    return jsonify({"message": "Coordinates received successfully!"}), 200

if __name__ == "__main__":
    server_ip = get_ip_address()
    print(f"[INFO] Flask server is running on http://{server_ip}:5000")
    app.run(host="0.0.0.0", port=5000, debug=True)
