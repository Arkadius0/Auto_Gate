#include "WebServerModule.h"
#include "config.h"
#include "Actuator.h"
#include <WebServer.h>
#include "GateControl.h"

// --- Server initialization ---
WebServer server(HTTP_PORT);

// --- Handlery HTTP ---
void handleRoot() {
  String html = "<!DOCTYPE html><html><head><meta charset='utf-8'><title>Gate Controller</title></head><body>"
                "<h2>ESP32 Auto Gate</h2>"
                "<p><a href='/open'><button>OPEN</button></a></p>"
                "<p><a href='/close'><button>CLOSE</button></a></p>"
                "<p>IP: " + WiFi.localIP().toString() + "</p>"
                "</body></html>";
  server.send(200, "text/html", html);
}

void handleOpen() {
  requestOpen();
  Serial.println("handleOpen() called");
  server.send(200, "text/plain", "Gate opening...");
}

void handleClose() {
  requestClose();
  Serial.println("handleClosed() called");
  server.send(200, "text/plain", "Gate closing...");
}

// --- Initializing the router and starting the server ---
void initWebServer() {
server.on("/", handleRoot);
server.on("/open", handleOpen);
server.on("/Open", handleOpen);
server.on("/close", handleClose);
server.on("/Close", handleClose);

  // support for unknown paths, including favicon.ico
  server.onNotFound([]() {
    if (server.uri() != "/favicon.ico") {
      Serial.print("Unknown request: ");
      Serial.println(server.uri());
    }
    server.send(404, "text/plain", "404: Not Found");
  });

  server.begin();
  Serial.println("Web server started.");
}

// --- Service in the main loop ---
void webServerLoop() {
  server.handleClient();
}
