import csv
import os
import subprocess
import sys
from datetime import datetime

import serial

BAUD_RATE = 9600
PORT = "COM4"

BASE_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
CSV_FILE = os.path.join(BASE_DIR, "data", "arduino_data.csv")
DASHBOARD_SCRIPT = os.path.join(BASE_DIR, "python", "update_dashboard.py")


def ensure_csv():
    os.makedirs(os.path.dirname(CSV_FILE), exist_ok=True)
    if not os.path.exists(CSV_FILE) or os.path.getsize(CSV_FILE) == 0:
        with open(CSV_FILE, "w", newline="", encoding="utf-8") as file:
            writer = csv.writer(file)
            writer.writerow([
                "Date",
                "Time",
                "Access",
                "Temperature_C",
                "Humidity_percent",
                "Distance_cm",
            ])


def update_dashboard():
    try:
        subprocess.run([sys.executable, DASHBOARD_SCRIPT], check=True)
        print("Dashboard updated.")
    except Exception as error:
        print(f"Dashboard update failed: {error}")


def save_record(access, temperature, humidity, distance):
    now = datetime.now()
    with open(CSV_FILE, "a", newline="", encoding="utf-8") as file:
        writer = csv.writer(file)
        writer.writerow([
            now.strftime("%Y-%m-%d"),
            now.strftime("%H:%M:%S"),
            access,
            temperature,
            humidity,
            distance,
        ])

    print("Saved:", access, temperature, humidity, distance)
    update_dashboard()


def main():
    ensure_csv()

    print(f"Connecting to Arduino on {PORT}...")

    try:
        board = serial.Serial(PORT, BAUD_RATE, timeout=1)
    except serial.SerialException as error:
        print(f"Could not open {PORT}: {error}")
        print("Close Arduino Serial Monitor before running this program.")
        return

    print("Connected. Waiting for Arduino data...")

    access = None
    temperature = None
    humidity = None
    distance = None

    try:
        while True:
            line = board.readline().decode("utf-8", errors="ignore").strip()
            if not line:
                continue

            print(line)

            if line == "ACCESS_GRANTED":
                access = "GRANTED"
                temperature = None
                humidity = None
                distance = None

            elif line == "ACCESS_DENIED":
                save_record("DENIED", "", "", "")
                access = None
                temperature = None
                humidity = None
                distance = None

            elif line.startswith("TEMPERATURE:") and access == "GRANTED":
                try:
                    temperature = float(line.split(":", 1)[1])
                except ValueError:
                    temperature = None

            elif line.startswith("HUMIDITY:") and access == "GRANTED":
                try:
                    humidity = float(line.split(":", 1)[1])
                except ValueError:
                    humidity = None

            elif line.startswith("DISTANCE:") and access == "GRANTED":
                value = line.split(":", 1)[1]
                try:
                    distance = float(value)
                except ValueError:
                    distance = "ERROR"

            if access == "GRANTED" and temperature is not None and humidity is not None and distance is not None:
                save_record("GRANTED", temperature, humidity, distance)
                access = None
                temperature = None
                humidity = None
                distance = None

    except KeyboardInterrupt:
        print("\nStopped by user.")
    finally:
        board.close()
        print("Arduino connection closed.")


if __name__ == "__main__":
    main()
