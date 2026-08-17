import os

from hardware.room_controller import RoomController


ESP32_IP = "192.168.1.12"


def clear_screen():
    os.system("cls" if os.name == "nt" else "clear")


def pause():
    input("\nPress Enter to return to menu...")


def show_menu():
    print("================================")
    print("   PHOENIX ROOM CONTROLLER V1")
    print("================================")
    print("1. Light ON")
    print("2. Light OFF")
    print("3. Get Light Status")
    print("4. Exit")
    print()


def main():
    clear_screen()

    print("Starting PHOENIX Room Controller...")
    print(f"Connecting to ESP32 at {ESP32_IP}...")

    controller = RoomController(ESP32_IP)

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
            else:
                print("Error:", result["error"])

            pause()
            clear_screen()

        elif choice == "2":
            result = controller.light_off()

            if result["success"]:
                print("Light turned OFF.")
            else:
                print("Error:", result["error"])

            pause()
            clear_screen()

        elif choice == "3":
            result = controller.get_light_status()

            if result["success"]:
                print(f"Light Status: {result['light']}")
            else:
                print("Error:", result["error"])

            pause()
            clear_screen()

        elif choice == "4":
            print("PHOENIX Room Controller stopped.")
            break

        else:
            print("Invalid option. Please select 1-4.")

            pause()
            clear_screen()


if __name__ == "__main__":
    main()