file=ToDo.md

1. App Queries Gateway Coordinates to the lib/api/ttn_api.dart:
when the app api reads the uplink message, it extracts the gateway_id or eui, and queries the LoRaWAN network server for the gateway's registered coordinates.

2. App Sends Gateway Coordinates to the Network Module:
Once the app api has retrieved the gateway's coordinates, it sends them back to the Network module in a json structured or a simple message.

3. Network Module Function of the Device Computes Estimated Coordinates:
The Network module receives the message, parses the gateway coordinates from the structured message, and uses them in the geolocation algorithm (e.g., trilateration) to compute the estimated device location.
