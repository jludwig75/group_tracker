#include "location_server.h"

#include "location_list.h"

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>


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
