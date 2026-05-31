#include <WiFi.h>
#include <esp_wifi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <Preferences.h>
#include <Arduino.h>
#include "Configs.h"
#include "USB_Tools.h"

DNSServer  dnsServer;
WebServer  server(80);
Preferences prefs;

// =============================================================
//  Conversion helpers
// =============================================================
static String macToStr(const uint8_t *m) {
  char b[18];
  snprintf(b, sizeof(b), "%02X:%02X:%02X:%02X:%02X:%02X", m[0], m[1], m[2], m[3], m[4], m[5]);
  return String(b);
}
static bool strToMac(const String &s, uint8_t *m) {
  unsigned int v[6];
  if (sscanf(s.c_str(), "%x:%x:%x:%x:%x:%x", &v[0], &v[1], &v[2], &v[3], &v[4], &v[5]) != 6) return false;
  for (int i = 0; i < 6; i++) m[i] = (uint8_t)v[i];
  return true;
}
static String jstr(const String &v) {
  String o = "\"";
  for (size_t i = 0; i < v.length(); i++) {
    char c = v[i];
    if (c == '"' || c == '\\') o += '\\';
    o += c;
  }
  o += "\"";
  return o;
}

// =============================================================
//  Persistence (NVS via Preferences)
// =============================================================
void loadSettings() {
  prefs.begin("usbware", true);
  inputSSID    = prefs.getString("ssid",  inputSSID);
  inputPASS    = prefs.getString("pass",  inputPASS);
  inputCH      = prefs.getInt   ("ch",    inputCH);
  inputHidden  = prefs.getBool  ("hidden",inputHidden);
  maxClients   = prefs.getInt   ("max",   maxClients);
  accessKey    = prefs.getString("akey",  accessKey);
  defaultDelay = prefs.getInt   ("ddelay",defaultDelay);
  stringDelay  = prefs.getInt   ("sdelay",stringDelay);
  strToMac(prefs.getString("mac", macToStr(MAC)), MAC);
  local_IP.fromString(prefs.getString("ip", local_IP.toString()));
  gateway .fromString(prefs.getString("gw", gateway.toString()));
  subnet  .fromString(prefs.getString("sn", subnet.toString()));
  prefs.end();
}
void saveSettings() {
  prefs.begin("usbware", false);
  prefs.putString("ssid",   inputSSID);
  prefs.putString("pass",   inputPASS);
  prefs.putInt   ("ch",     inputCH);
  prefs.putBool  ("hidden", inputHidden);
  prefs.putInt   ("max",    maxClients);
  prefs.putString("akey",   accessKey);
  prefs.putInt   ("ddelay", defaultDelay);
  prefs.putInt   ("sdelay", stringDelay);
  prefs.putString("mac",    macToStr(MAC));
  prefs.putString("ip",     local_IP.toString());
  prefs.putString("gw",     gateway.toString());
  prefs.putString("sn",     subnet.toString());
  prefs.end();
}

// =============================================================
//  Authorization: when accessKey is set, /set and settings POST
//  require a matching X-USBWARE-KEY header (or ?key= form field).
// =============================================================
bool authorized() {
  if (accessKey.length() == 0) return true;
  if (server.hasHeader("X-USBWARE-KEY") && server.header("X-USBWARE-KEY") == accessKey) return true;
  if (server.hasArg("key") && server.arg("key") == accessKey) return true;
  return false;
}

// =============================================================
//  Handlers
// =============================================================
void handleSet() {
  if (!authorized()) { server.send(401, "text/plain", "Unauthorized: missing or bad key"); return; }
  String script = server.hasArg("customScript") ? server.arg("customScript") : "";
  DuckyScript(script);
  server.send(200, "text/plain", "Executed successfully.");
}

void handleGetSettings() {
  if (!authorized()) { server.send(401, "text/plain", "Unauthorized"); return; }
  String j = "{";
  j += "\"ssid\":"   + jstr(inputSSID)        + ",";
  j += "\"pass\":"   + jstr(inputPASS)        + ",";
  j += "\"ch\":"     + String(inputCH)        + ",";
  j += "\"hidden\":" + String(inputHidden ? "true" : "false") + ",";
  j += "\"max\":"    + String(maxClients)     + ",";
  j += "\"mac\":"    + jstr(macToStr(MAC))    + ",";
  j += "\"ip\":"     + jstr(local_IP.toString()) + ",";
  j += "\"gw\":"     + jstr(gateway.toString())  + ",";
  j += "\"sn\":"     + jstr(subnet.toString())   + ",";
  j += "\"akey\":"   + jstr(accessKey)        + ",";
  j += "\"ddelay\":" + String(defaultDelay)   + ",";
  j += "\"sdelay\":" + String(stringDelay)    + ",";
  j += "\"clients\":"+ String(WiFi.softAPgetStationNum()) + ",";
  j += "\"heap\":"   + String(ESP.getFreeHeap());
  j += "}";
  server.send(200, "application/json", j);
}

void handlePostSettings() {
  if (!authorized()) { server.send(401, "text/plain", "Unauthorized"); return; }
  if (server.hasArg("ssid"))   inputSSID    = server.arg("ssid");
  if (server.hasArg("pass"))   inputPASS    = server.arg("pass");
  if (server.hasArg("ch"))     inputCH      = constrain((int)server.arg("ch").toInt(), 1, 13);
  if (server.hasArg("max"))    maxClients   = constrain((int)server.arg("max").toInt(), 1, 8);
  if (server.hasArg("hidden")) inputHidden  = (server.arg("hidden") == "1" || server.arg("hidden") == "true");
  if (server.hasArg("akey"))   accessKey    = server.arg("akey");
  if (server.hasArg("ddelay")) defaultDelay = max(0L, (long)server.arg("ddelay").toInt());
  if (server.hasArg("sdelay")) stringDelay  = max(0L, (long)server.arg("sdelay").toInt());
  if (server.hasArg("mac"))    strToMac(server.arg("mac"), MAC);
  if (server.hasArg("ip"))     local_IP.fromString(server.arg("ip"));
  if (server.hasArg("gw"))     gateway.fromString(server.arg("gw"));
  if (server.hasArg("sn"))     subnet.fromString(server.arg("sn"));

  saveSettings();

  bool reboot = server.hasArg("reboot") && server.arg("reboot") == "1";
  server.send(200, "text/plain", reboot ? "Saved. Rebooting to apply..." : "Saved (network changes need a reboot).");
  if (reboot) { delay(400); ESP.restart(); }
}

// =============================================================
//  Access point + captive portal
// =============================================================
void startAP() {
  WiFi.mode(WIFI_AP);
  esp_wifi_set_mac(WIFI_IF_AP, &MAC[0]);
  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.softAP(inputSSID.c_str(), inputPASS.c_str(), inputCH, inputHidden, maxClients);

  dnsServer.start(DNS_PORT, "*", local_IP);

  const char *collect[] = { "X-USBWARE-KEY" };
  server.collectHeaders(collect, 1);

  server.on("/", HTTP_GET, []() { server.send(200, "text/html", MainPage); });
  server.on("/set", HTTP_POST, handleSet);
  server.on("/api/settings", HTTP_GET,  handleGetSettings);
  server.on("/api/settings", HTTP_POST, handlePostSettings);
  server.onNotFound([]() { server.send(200, "text/html", MainPage); });

  auto redirect = []() {
    server.sendHeader("Location", String("http://") + local_IP.toString(), true);
    server.send(302, "text/plain", "");
  };
  const char *detectionPaths[] = {
    "/generate_204", "/hotspot-detect.html", "/library/test/success.html",
    "/connecttest.txt", "/success.txt", "/ncsi.txt", "/index.html", "/fwlink"
  };
  for (auto path : detectionPaths) server.on(path, HTTP_GET, redirect);

  server.begin();
}

void setup() {
  loadSettings();
  startUSB("Both");
  startAP();
}

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();
}