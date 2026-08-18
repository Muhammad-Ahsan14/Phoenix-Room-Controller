import json
import os

from hardware.room_controller import RoomController


CONFIG_FILE = "config.json"


def clear_screen():
    os.system("cls" if os.name == "nt" else "clear")


def pause():
    input("\nPress Enter to return to menu...")


def load_config():
    try:
        with open(CONFIG_FILE, "r") as file:
            return json.load(file)

    except FileNotFoundError:
        print(f"Configuration file '{CONFIG_FILE}' was not found.")
        print("Copy config.example.json to config.json and add your ESP32 IP address.")
        return None

    except json.JSONDecodeError:
        print(f"Configuration file '{CONFIG_FILE}' contains invalid JSON.")
        return None


def show_menu():
    print("================================")
    print("   PHOENIX ROOM CONTROLLER V2")
    print("================================")
    print("1. Light ON")
    print("2. Light OFF")
    print("3. Get Light Status")
    print("4. Exit")
    print()


def main():
    clear_screen()

    config = load_config()

    if config is None:
        return

    esp32_ip = config.get("esp32_ip")

    if not esp32_ip:
        print("ESP32 IP address is missing from config.json.")
        return

    controller = RoomController(esp32_ip)

    print("Starting PHOENIX Room Controller V2...")
    print(f"Connecting to ESP32 at {esp32_ip}...")

    connection = controller.check_connection()

    if not connection["success"]:
        print("\nConnection failed.")
        print(connection["error"])
        return

    clear_screen()

    while True:
        show_menu()

        choice = input("Select option: ").strip()

        clear_screen()

        if choice == "1":
            result = controller.light_on()

            if result["success"]:
                print("Light turned ON.")
                print("Relay State: ON")
            else:
                print("Error:", result["error"])

            pause()
            clear_screen()

        elif choice == "2":
            result = controller.light_off()

            if result["success"]:
                print("Light turned OFF.")
                print("Relay State: OFF")
            else:
                print("Error:", result["error"])

            pause()
            clear_screen()

        elif choice == "3":
            result = controller.get_light_status()

            if result["success"]:
                print(f"Light Status: {result['light']}")
                print(f"Relay State: {result['relay']}")
            else:
                print("Error:", result["error"])

            pause()
            clear_screen()

        elif choice == "4":
            print("PHOENIX Room Controller V2 stopped.")
            break

        else:
            print("Invalid option. Please select 1-4.")
            pause()
            clear_screen()


if __name__ == "__main__":
    main()