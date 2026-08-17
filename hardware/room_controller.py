import requests


class RoomController:
    def __init__(self, ip_address):
        self.base_url = f"http://{ip_address}"
        self.timeout = 3

    def check_connection(self):
        return self._send_request("/")

    def light_on(self):
        return self._send_request("/light/on")

    def light_off(self):
        return self._send_request("/light/off")

    def get_light_status(self):
        return self._send_request("/light/status")

    def _send_request(self, endpoint):
        try:
            response = requests.get(
                self.base_url + endpoint,
                timeout=self.timeout
            )

            response.raise_for_status()

            data = response.json()

            if isinstance(data, dict):
                data["success"] = True

            return data

        except requests.exceptions.ConnectionError:
            return {
                "success": False,
                "error": "Could not connect to PHOENIX device."
            }

        except requests.exceptions.Timeout:
            return {
                "success": False,
                "error": "PHOENIX device timed out."
            }

        except requests.exceptions.RequestException as error:
            return {
                "success": False,
                "error": str(error)
            }

        except ValueError:
            return {
                "success": False,
                "error": "Invalid response received from PHOENIX device."
            }