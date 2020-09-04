#include <WiFi.h>
#include <WebSocketsServer.h>

// Constants
const char* ssid = "";
const char* password = "";
int result = 2;

// Globals
WebSocketsServer webSocket = WebSocketsServer(80);

// Called when receiving any WebSocket message
void onWebSocketEvent(uint8_t num,
                      WStype_t type,
                      uint8_t * payload,
                      size_t length) {

  // Figure out the type of WebSocket event
  switch(type) {

    // Client has disconnected
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;

    // New client has connected
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connection from ", num);
        Serial.println(ip.toString());
        webSocket.sendTXT(0, "true");
      }
      break;

    // Echo text message back to client
    case WStype_TEXT:
      Serial.printf("[%u] Text: %s\n", num, payload);
      result = strcmp((char *) payload, "true");
      Serial.println(result);

      if(result == 0){
        digitalWrite(2, HIGH);
        webSocket.sendTXT(0, "true");
      }else{
        digitalWrite(2, LOW);
        webSocket.sendTXT(0, "false");
      }
     
      break;

    // For everything else: do nothing
    case WStype_BIN:
    case WStype_ERROR:
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
    default:
      break;
  }
}

void setup() {

  // Start Serial port
  Serial.begin(115200);

  // Connect to access point
  Serial.println("Connecting");
  WiFi.begin(ssid, password);
  while ( WiFi.status() != WL_CONNECTED ) {
    delay(500);
    Serial.print(".");
  }

  // Print our IP address
  Serial.println("Connected!");
  Serial.print("My IP address: ");
  Serial.println(WiFi.localIP());

  // Start WebSocket server and assign callback
  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);

  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
}

void loop() {

  // Look for and handle WebSocket data
  webSocket.loop();
}
