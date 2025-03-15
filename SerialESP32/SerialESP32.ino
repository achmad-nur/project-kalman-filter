#include <WiFi.h>
#include <HTTPClient.h>

// WiFi dan Password
// const char* ssid = "Kontrakan Sultan Lt1";
// const char* pass = "AyoSkripsi123";
const char* ssid = "POCO F4";
const char* pass = "qwerty123456";

// Host Variabel
// const char* host = "192.168.1.12";  // Ganti dengan alamat IP server PHP Anda
// const char* host = "192.168.67.234";  // Ganti dengan alamat IP server PHP Anda
const char* host = "192.168.36.234";  // Ganti dengan alamat IP server PHP Anda

#define RXD2 16  // Pin RX2 ESP32 terhubung ke TX1 Arduino Mega
#define TXD2 17  // Pin TX2 ESP32 terhubung ke RX1 Arduino Mega

void setup() {
  Serial.begin(115200);                           // Komunikasi serial dengan komputer melalui USB
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);  // Komunikasi serial dengan Arduino Mega

  // Koneksi ke WiFi
  WiFi.begin(ssid, pass);
  Serial.println("Mencoba Terkoneksi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("...");
    delay(500);
  }
  Serial.println("Terkoneksi ke WiFi!");
}

void loop() {
  // Mengecek apakah ada data yang tersedia dari Arduino Mega
  if (Serial2.available() > 0) {
    String receivedData = Serial2.readStringUntil('\n');  // Membaca data yang dikirim dari Arduino Mega

    // Menampilkan data yang diterima di Serial Monitor
    Serial.print("Data diterima: ");
    Serial.println(receivedData);

    // Memisahkan data yang diterima berdasarkan koma
    int firstCommaIndex = receivedData.indexOf(',');
    int secondCommaIndex = receivedData.indexOf(',', firstCommaIndex + 1);

    String voltage = receivedData.substring(0, firstCommaIndex);
    String pH = receivedData.substring(firstCommaIndex + 1, secondCommaIndex);
    String kalman = receivedData.substring(secondCommaIndex + 1);

    // Trim leading and trailing spaces
    voltage.trim();
    pH.trim();
    kalman.trim();

    // Menampilkan data individual di Serial Monitor
    Serial.println(voltage);
    Serial.println(pH);
    Serial.println(kalman);

    // Kirim data ke server
    WiFiClient client;
    const int httpPort = 80;
    if (!client.connect(host, httpPort)) {
      Serial.println("Gagal Terkoneksi ke Server!");
      return;
    }

    String link;
    HTTPClient http;

    // Buat URL untuk request HTTP GET dengan parameter yang sesuai
    link = "http://" + String(host) + "/smarthidroponik/kirimdata.php?ph=" + pH + "&kalman=" + kalman;

    Serial.println("URL yang dikirim: " + link);

    http.begin(link);
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Respon dari server: " + response);
    } else {
      Serial.println("Error pada pengiriman GET: " + String(httpResponseCode));
    }

    http.end();
  }
  delay(1000);  // Delay untuk mencegah pengiriman terlalu cepat
}