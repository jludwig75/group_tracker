#include "location_server.h"

#ifdef  ESP32

#include "location_list.h"

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>


const char* ssid = "Caradhras";
const char* password = "Speak friend.";

WebServer server(80);


void handleRoot()
{
    server.send(200, "text/plain", "hello from esp8266!");
}

void handleNotFound()
{
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    for (uint8_t i = 0; i < server.args(); i++)
    {
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    server.send(404, "text/plain", message);
}

static LocationServer *s_location_server = NULL;

void handleListLocations()
{
    if (s_location_server)
    {
        String message = s_location_server->handle_list_locations();
        server.send(200, "text/plain", message);
    }
}

LocationServer::LocationServer(const LocationList &location_list) :
    _location_list(location_list)
{
    s_location_server = this;
}

String LocationServer::handle_list_locations() const
{
  String message = "[";

  const Location * loc = _location_list.get_fist_location();
  while(loc)
  {
      message += "{";
      message += String("\"station_id\": ") + loc->get_station_id();
      message += String(", \"timestamp\": ") + loc->get_timestamp();
      message += String(", \"latitude\": ") + String(loc->get_latitude(), 6);
      message += String(", \"longitude\": ") + String(loc->get_longitude(), 6);
      message += "}";
      loc = _location_list.get_next_location(loc);
      if (loc)
      {
          message += ",\n";
      }
  }

  message += "]";

  return message;
}

void LocationServer::begin()
{
    // Start WiFi
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.println("");
  
    // Wait for connection
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  
    if (MDNS.begin("akela"))
    {
        Serial.println("MDNS responder started");
    }
    
    // Start web server
    server.on("/", handleRoot);

    server.on("/locs", handleListLocations);

    server.onNotFound(handleNotFound);
  
    server.begin();
    Serial.println("HTTP server started");
}

void LocationServer::on_loop()
{
    server.handleClient();
}

#else // ESP32
LocationServer::LocationServer(const LocationList &location_list) :
    _location_list(location_list)
{
}

void LocationServer::begin()
{
}

void LocationServer::on_loop()
{
}
#endif // ESP32
