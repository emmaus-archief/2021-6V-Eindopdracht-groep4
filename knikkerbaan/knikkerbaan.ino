# include <SPI.h>
# include <WiFiNINA.h>
# include <utility/wifi_drv.h>

char ssid [] = "naam-van-je-netwerk"; // naam van je wifi-netwerk
char pass [] = "wachtw00rd"; // wifi-wachtwoord

int status = WL_IDLE_STATUS;

char server [] = "3000-tomato-quokka-0dxmfooo.ws-eu03.gitpod.io";

WiFiSSLClient client;

long statusTimerLaatsteActivering = 0;
int  statusTimerWachtTijd = 1000;  // elke seconde 'tellen' we een knikker

long wifiTimerLaatsteActivering = 0;
int  wifiTimerWachtTijd = 30000; // elke 30 seconden sturen we een update naar de server

int baanStatus = 0;

# include <Servo.h>

Servo servo;

// variabelen voor de sensor
int pinLaser = 7;
int pinReceiver = 6;

// variabelen voor de lampjes 
int rood = 13;
int blauw = 12;
int geel = 11;
int groen = 10;

// variabele voor tijd
int huidigeMillis = 0;

// variabele voor tellen van hoeveel balletjes voorbij komen
int aantalBalletjes = 0;

void setup () {
 
  setWiFiLED(255, 0, 0); // laat LED van wifi rood branden
  Serial.begin (9600); // seriele poort klaarzetten
  while (!Serial) { // wachten tot de seriele poort verbinding heeft
    ;
  } 
  
  // verbind met WiFi
  //checkWiFiModule ();
  //verbindMetWiFiNetwerk ();
  
  setWiFiLED (0,255, 0); // verander de kleur van de LED van de WiFi naar groen

  // initialiseer hier je timers:
  // zet de start, t=0, van de timers op NU,
  // zodat die 1 en 30 seconden vanaf NU voor het eerst 'afgaan'.
  statusTimerLaatsteActivering = millis();
  wifiTimerLaatsteActivering=  millis();
  
  // set up van de sensor
  pinMode (pinLaser, OUTPUT);
  pinMode (pinReceiver, INPUT);
  digitalWrite(pinLaser, HIGH);
  
  //setup van de lampjes
  pinMode (rood, OUTPUT);
  pinMode (blauw, OUTPUT);
  pinMode (geel, OUTPUT);
  pinMode (groen, OUTPUT);
 
  // setup van de servo
  servo.attach(9);
}

void loop () {
  /*
  // controleer of de telTimer 'af moet gaan'
  if (millis() > statusTimerLaatsteActivering + statusTimerWachtTijd) {
    // er zijn blijkbaar meer van 1000 milliseconden verstreken
    // sinds de laatste activering van de telTimer
    // Verhoog knikkerTeller met 1.
    // update de tijd van de laatste activering naar NU
    statusTimerLaatsteActivering = millis();
    
    // geef de baanstatus een random waarde tussen 0 en 10:
    baanStatus = random(10);
  }

  // controleer of de wifiTimer 'af moet gaan'
  if (millis() > wifiTimerLaatsteActivering + wifiTimerWachtTijd) {
    // er zijn blijkbaar meer van 30000 milliseconden verstreken
    // sinds de laatste activering van de wifiTimer
    
    // update de tijd van de laatste activering naar NU
    wifiTimerLaatsteActivering = millis();

    // tijdens communicatie zetten we de LED even op blauw
    setWiFiLED(0, 0, 255);
    
    stuurUpdate();
  }


  // Je wilt in ieder geval kunnen lezen wat de
  // server als antwoord terugstuurt.
  if (client.connected()) {
    checkHTTPResponse();

    // na antwoord zetten we LED weer op groen:
    setWiFiLED(0, 255, 0);
  }
  
  // hele korte delay om de Arduino niet helemaal gek te maken
  delay(50);
  */
  // variabele declareren, door het uitlezen van de pin
  int value = digitalRead (pinReceiver); 
  
  // variabele declareren met millis
  int huidigeMillis = millis();
  
  // als er aan deze tijdaanduidingen wordt voldaan, zal de servo op 0 graden gedraaid blijven staan 
  if (huidigeMillis > 0 && huidigeMillis < 600) {
    servo.write(0);
  }
  
  // als er aan deze tijdsaanduiding is voldaan, zal de servo zichzelf naar 90 graden draaien
  if (huidigeMillis >= 600) {
    servo.write(90);
  }
  
  // als er aan deze tijdsaanduiding is voldaan, zal de servo zichzelf weer terugdraaien naar 0 graden
  if (huidigeMillis >= 1200) {
    servo.write(0);
  }
  
  // als er aan deze tijdsaanduiding is voldaan, zal de servo zichzelf naar 90 graden draaien
  if (huidigeMillis >= 1800) {
    servo.write(90);
  }
  
  // als er aan deze tijdsaanduiding is voldaan, zal de servo zichzelf weer terugdraaien naar 0 graden
  if (huidigeMillis >= 2400) {
    servo.write(0);
  }
  
  // als er een balletje voorbij komt, receiver ontvangt niets
  if (value == LOW) {
    digitalWrite (rood, HIGH);
    digitalWrite (blauw, HIGH);
    digitalWrite (geel, HIGH);
    digitalWrite (groen, HIGH);
  }
  
  // als er geen balletje voorbij komt, receiver ontvangt laserstraal
  if (value == HIGH) {
    digitalWrite (rood, LOW);
    digitalWrite (blauw, LOW);
    digitalWrite (geel, LOW);
    digitalWrite (groen, LOW);
  }
    
  /* als er een balletje voorbij komt, receiver ontvangt niets, 
  tellen we dit balletje op bij het aantalBalletjes, zo kunnen we zien hoeveel balletjes er langs zijn geweest. 
  eigenlijk wilde we dit op de widget weergeven, maar dit lukte niet, dus geven we het weer in de Serial Monitor */
  if (value == LOW) {
    aantalBalletjes = aantalBalletjes + 1;
    Serial.println(aantalBalletjes);
  }
}

void stuurUpdate() {
  Serial.println("\nStart verbinding met server");

  // hier maken we gebruik van het client-object om
  // de verbinding te maken. Die geeft true of false
  // terug om aan te geven of het is gelukt.
  // Het kan even duren voor de verbinding is gemaakt.
  if (client.connect(server, 443)) {
    Serial.println("Verbonden met de server. HTTP verzoek wordt verstuurd.");
    
    // We sturen nu m.b.v. het client-object de tekst van een HTTP verzoek:
    // 1e regel
    client.print("GET /api/setKnikkerbaanStatus/");
    client.print(baanStatus);
    client.println(" HTTP/1.1");

    // 2e regel
    client.print("Host: ");
    client.println(server);

    // 3e regel
    client.println("Connection: close");

    // 4e regel -- moet leeg zijn
    client.println();

    /* Dat verzoek ziet er dus zo uit als baanstatus de waarde 8 heeft:
     GET /api/setKnikkerbaanStatus/8 HTTP/1.1
     Host: 3000-blablabla.gitpod.io
     Connection: close
     
    */
  }
  else {
    Serial.println("verbinding maken niet gelukt");
  }
  
}

void checkHTTPResponse() {
  // controleer of de client data beschikbaar heeft
  // lees deze uit en print ze, net zolang tot er
  // geen nieuwe data meer beschikbaar is
  while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }

  // mocht de server na het antwoord toch zelf niet
  // de verbinding hebben verbroken, doe het dan maar zelf
  if (!client.connected()) {
    Serial.println("");
    Serial.println("---------EINDE ANTWOORD VAN DE SERVER----------");
    Serial.println("De verbinding met de server is verbroken");
    client.stop();
  }
}

void checkWiFiModule() {
  // controleer of er een WiFi module is
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  // controleer de versie van de firmware
  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }
}

void verbindMetWiFiNetwerk() {
  // probeer contact te maken met het opgegeven WiFi-netwerk:
  // dit gaat met een while-loop, zodat je niet verder kunt zolang
  // je niet verbonden bent.
  while (status != WL_CONNECTED) {
    Serial.print("Probeert te verbinden met SSID: ");
    Serial.println(ssid);
    
    // verbind met WiFi.begin
    status = WiFi.begin(ssid, pass);

    // wacht 4 seconden voor de verbinding:
    // mocht het verbinden niet lekker gaan, probeer er dan
    // 10000 milliseconden van te maken (10 seconden)
    delay(4000);
  }
  // als je hier komt, betekent dat dat je verbonden bent.  
  Serial.println("Verbonden met WiFi");

  // print informatie over het netwerk waarmee je verbonden bent
  printWiFiStatus();
}

void printWiFiStatus() {
  // print de naam van het netwerk waarmee je verbonden bent:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print het IP-adres dat je Arduino heeft gekregen:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP-adres: ");
  Serial.println(ip);

  // print de sterkte van het signaal:
  long rssi = WiFi.RSSI();
  Serial.print("Signaalsterkte (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}


// stel de kleur van de RGB led in
void setWiFiLED(uint8_t rood, uint8_t groen, uint8_t blauw) {
  WiFiDrv::pinMode(25, OUTPUT);
  WiFiDrv::pinMode(26, OUTPUT);
  WiFiDrv::pinMode(27, OUTPUT);
  WiFiDrv::analogWrite(25, rood);
  WiFiDrv::analogWrite(26, groen);
  WiFiDrv::analogWrite(27, blauw);
}
