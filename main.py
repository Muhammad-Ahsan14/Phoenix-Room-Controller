import json
import os
import time
import msvcrt

from hardware.room_controller import RoomController


CONFIG_FILE = "config.json"

STATUS_REFRESH_INTERVAL = 0.5


def clear_screen():
    os.system(
        "cls"
        if os.name == "nt"
        else "clear"
    )


def load_config():
    try:
        with open(
            CONFIG_FILE,
            "r"
        ) as file:
            return json.load(file)

    except FileNotFoundError:
        print(
            f"Configuration file "
            f"'{CONFIG_FILE}' "
            f"was not found."
        )

        print(
            "Copy config.example.json "
            "to config.json and add "
            "your ESP32 IP address."
        )

        return None

    except json.JSONDecodeError:
        print(
            f"Configuration file "
            f"'{CONFIG_FILE}' "
            f"contains invalid JSON."
        )

        return None


def show_menu():
    print(
        "================================"
    )

    print(
        "   PHOENIX ROOM CONTROLLER V3"
    )

    print(
        "================================"
    )

    print(
        "1. Light ON"
    )

    print(
        "2. Light OFF"
    )

    print(
        "3. Live System Status"
    )

    print(
        "4. Exit"
    )

    print()


def show_result(result):
    print(
        f"Commanded State : "
        f"{result['commanded']:<20}"
    )

    print(
        f"Actual State    : "
        f"{result['actual']:<20}"
    )

    print(
        f"System Status   : "
        f"{result['system_status']:<20}"
    )

    print(
        f"Verification    : "
        f"{result['detail']:<30}"
    )

    print(
        f"LDR Reading     : "
        f"{result['ldr_value']:<20}"
    )


def enter_pressed():
    if not msvcrt.kbhit():
        return False

    key = msvcrt.getwch()

    return key == "\r"


def live_status_screen(
    controller,
    heading
):
    clear_screen()

    # Hide terminal cursor
    print(
        "\033[?25l",
        end=""
    )

    try:
        while True:

            result = (
                controller
                .get_light_status()
            )

            # Move cursor back
            # to top-left.
            # Do not clear entire
            # terminal every refresh.
            print(
                "\033[H",
                end=""
            )

            print(
                f"{heading:<55}"
            )

            print(
                "=" * len(heading)
            )

            print()


            if result["success"]:

                show_result(
                    result
                )

            else:

                print(
                    "Connection Error"
                    "                                       "
                )

                print(
                    f"{result['error']:<55}"
                )

                print(
                    "                                                       "
                )

                print(
                    "                                                       "
                )

                print(
                    "                                                       "
                )


            print()

            print(
                "Live monitoring active "
                f"(refresh: "
                f"{STATUS_REFRESH_INTERVAL}s)"
                "          "
            )

            print(
                "Press Enter to return "
                "to menu..."
                "                    "
            )


            elapsed = 0.0


            while (
                elapsed <
                STATUS_REFRESH_INTERVAL
            ):

                if enter_pressed():
                    return

                time.sleep(
                    0.05
                )

                elapsed += 0.05

    finally:

        # Show terminal cursor again
        print(
            "\033[?25h",
            end=""
        )

        clear_screen()


def pause():
    input(
        "\nPress Enter to return "
        "to menu..."
    )

    clear_screen()


def main():
    clear_screen()


    config = load_config()


    if config is None:
        return


    esp32_ip = config.get(
        "esp32_ip"
    )


    if not esp32_ip:

        print(
            "ESP32 IP address is "
            "missing from config.json."
        )

        return


    controller = RoomController(
        esp32_ip
    )


    print(
        "Starting PHOENIX "
        "Room Controller V3..."
    )


    print(
        f"Connecting to ESP32 "
        f"at {esp32_ip}..."
    )


    connection = (
        controller
        .check_connection()
    )


    if not connection["success"]:

        print()

        print(
            "Connection failed."
        )

        print(
            connection["error"]
        )

        return


    clear_screen()


    while True:

        show_menu()


        choice = input(
            "Select option: "
        ).strip()


        clear_screen()


        if choice == "1":

            result = (
                controller
                .light_on()
            )


            if result["success"]:

                live_status_screen(
                    controller,
                    "LIGHT ON COMMAND EXECUTED"
                )

            else:

                print(
                    "Error:",
                    result["error"]
                )

                pause()


        elif choice == "2":

            result = (
                controller
                .light_off()
            )


            if result["success"]:

                live_status_screen(
                    controller,
                    "LIGHT OFF COMMAND EXECUTED"
                )

            else:

                print(
                    "Error:",
                    result["error"]
                )

                pause()


        elif choice == "3":

            live_status_screen(
                controller,
                "PHOENIX LIVE SYSTEM STATUS"
            )


        elif choice == "4":

            print(
                "PHOENIX Room Controller "
                "V3 stopped."
            )

            break


        else:

            print(
                "Invalid option. "
                "Please select 1-4."
            )

            pause()


if __name__ == "__main__":
    main()