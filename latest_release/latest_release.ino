#include <WiFi.h>
#include <esp_wifi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <Arduino.h>
#include "Configs.h"
#include "USB_Tools.h"

DNSServer dnsServer;
WebServer server(80);

void handleSet() {
  String feature = server.hasArg("feature") ? server.arg("feature") : "";
  String customScript = server.hasArg("customScript") ? server.arg("customScript") : "";

  if (feature == "Richroll") {
    pingSite();
  } else if (feature == "Python Script") {
      py_script();
  } else if (feature == "Script + Ping") {
      py_script();
      pingSite();
  } else if (feature == "Barrel Roll") {
      barrelRollMode();
  } else if (feature == "Notepad") {
      notepadMode();
  } else if (feature == "All") {
      py_script();
      delay(1000);
      barrelRollMode();
      delay(3000);
      pingSite();
      delay(1000);
      notepadMode();
  } else if (feature == "Custom Script") {
      runDuckyScript(customScript);
  }

  server.send(200, "text/plain", feature + " Executed successfully.");
}

void startAP() {
  WiFi.mode(WIFI_AP);
  esp_wifi_set_mac(WIFI_IF_AP, &MAC[0]);
  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.softAP(inputSSID.c_str(), inputPASS.c_str(), inputCH, inputHidden, maxClients);

  // Start DNS server: redirect all domains to ESP32
  dnsServer.start(DNS_PORT, "*", local_IP);

  // Serve root page and fallback for all unknown URLs
  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", MainPage);
  });
  server.onNotFound([]() {
    server.send(200, "text/html", MainPage);
  });
  // Login form handler
  server.on("/set", HTTP_POST, handleSet);

  // ---- redirect handler ----
  auto redirectHandler1 = []() {
    server.sendHeader("Location", String("http://") + local_IP.toString(), true);
    server.send(302, "text/plain", "");
  };

  // List of common captive portal detection paths
  const char* detectionPaths1[] = {
    "/generate_204",              // Android
    "/hotspot-detect.html",       // iOS/macOS
    "/library/test/success.html", // older iOS
    "/connecttest.txt",           // Windows
    "/success.txt",               // Windows
    "/ncsi.txt",                  // Windows 10/11
    "/index.html",                // Linux
    "/fwlink"                     // Linux
  };

  // Attach redirect handler to all detection paths
  for (auto path : detectionPaths1) {
    server.on(path, HTTP_GET, redirectHandler1);
  }

  server.begin();
}

void setup() {
  startUSB("Keyboard");
  startAP();
}

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();
}
