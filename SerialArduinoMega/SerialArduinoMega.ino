#define PH_PIN A0  // Pin analog untuk pembacaan pH sensor

// Deklarasi pin motor
int enA = 11;
int in_1 = 28;
int in_2 = 26;
int enB = 8;
int in_3 = 24;
int in_4 = 22;

// Inisialisasi read Sensor
const int samples = 10;
float adc_resolution = 1023.0;  // Resolusi ADC Arduino Mega
float pHVolt, tempValue;
float calibration_factor = 2.45 / 2.50;  // Faktor kalibrasi berdasarkan perbandingan
float reference_voltage = 5.0;           // Referensi tegangan untuk Arduino Mega
float Po = 0;

// Inisialisasi Kalman Filter
float SensorData, KalmanFilterData;
float Xt, Xt_update, Xt_prev;
float Pt, Pt_update, Pt_prev;
float Kt, R, Q;

float pHnetral_basa = 6.5;
float pHnetral_asam = 5.5;
float pH_target;

unsigned long previousMillis = 0;
unsigned long pumpDuration = 0;
bool isPumping = false;
int pumpSpeed = 0;
int pumpType = 0;  // 1: pompa asam, 2: pompa basa

void setup() {
  Serial.begin(9600);
  Serial1.begin(115200);  // Serial1 digunakan untuk komunikasi dengan ESP32

  pinMode(enA, OUTPUT);
  pinMode(in_1, OUTPUT);
  pinMode(in_2, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in_3, OUTPUT);
  pinMode(in_4, OUTPUT);

  // pH
  pinMode(PH_PIN, INPUT);

  // Inisialisasi Variabel R dan Q
  R = 100;  // Mengurangi nilai noise sensor untuk respons lebih cepat
  Q = 0.1;    // Meningkatkan nilai Q untuk mempercepat konvergensi filter
  Pt_prev = 1;

  // Inisialisasi nilai Xt_prev mendekati nilai sensor awal
  readSensor();
  Xt_prev = ph(pHVolt).toFloat();  // Estimasi awal mendekati nilai sensor
}

String ph(float voltage) {
  // Po = (-9.0781 * voltage) + 36.46; 
  Po = (-1.8115 * voltage) + 12.058; //daya motor
  // Po = (-10.792* voltage) + 42.293; //daya motor
  // Po = (-6.7154 * voltage) + 29.381;  

  // Po = (-9.2575 * voltage) + 37.477; //daya full

  Po = constrain(Po, 1.0, 14.0);
  return String(Po, 2);
}

void readSensor() {
  int measurings = 0;
  for (int i = 0; i < samples; i++) {
    measurings += analogRead(PH_PIN);
    delay(10);
  }
  float analog = measurings / samples;
  pHVolt = analog * (reference_voltage / adc_resolution) * calibration_factor;
  // Serial.print("Voltage: ");
  // Serial.print(pHVolt);
}

void sendData() {
  String phData = ph(pHVolt);  // Simpan nilai pH di variabel phData
  String dataToSend = "Voltage: " + String(pHVolt) + ", pH: " + phData + ", kalman: " + String(KalmanFilterData);
  String dataToSend1 = String(pHVolt) + "," + phData + "," + String(KalmanFilterData);

  // Kirim data ke ESP32 melalui Serial1
  Serial1.println(dataToSend1);

  // Tampilkan data yang dikirim ke Serial Monitor (debugging)
  // Serial.println(dataToSend);
}

void KalmanFilter() {
  readSensor();  // Baca nilai sensor
  String nilaiPH = ph(pHVolt);
  float pHValue = nilaiPH.toFloat();

  SensorData = pHValue;
  Xt_update = Xt_prev;
  Pt_update = Pt_prev + Q;
  Kt = Pt_update / (Pt_update + R);
  Xt = Xt_update + (Kt * (SensorData - Xt_update));
  Pt = (1 - Kt) * Pt_update;

  Xt_prev = Xt;
  Pt_prev = Pt;

  KalmanFilterData = Xt;

  Serial.print("Sensor: ");
  Serial.print(SensorData, 2);
  Serial.print(", ");
  Serial.print("Kalman: ");
  Serial.print(KalmanFilterData, 2);
  Serial.println();

  sendData();  // Kirimkan data setelah melakukan filter Kalman
  delay(2000);
}

void pump_1(int speed) {    // pompa asam
  analogWrite(enA, speed);  //mengatur tegangan ke motor sebesar 20(0-255) dari tegangan input
  //menentukan arah putar
  digitalWrite(in_1, HIGH);
  digitalWrite(in_2, LOW);
  Serial.println("Pompa 1 basa jalan...");
}
void pump_2(int speed) {    // pompa basa
  analogWrite(enB, speed);  //mengatur tegangan ke motor sebesar 20(0-255) dari tegangan input
  //menentukan arah putar
  digitalWrite(in_3, HIGH);
  digitalWrite(in_4, LOW);
  Serial.println("Pompa 2 asam jalan...");
}
void stopMotor_2() {  // pompa basa
  // Serial.println("Mematikan motor 2...");
  analogWrite(enB, 0);  // Mematikan suplai daya ke motor B
  digitalWrite(in_3, LOW);
  digitalWrite(in_4, LOW);
  Serial.println("Pompa 2 asam mati");
}
void stopMotor_1() {
  // Serial.println("Mematikan motor 1...");
  analogWrite(enA, 0);  // Mematikan suplai daya ke motor A
  digitalWrite(in_1, LOW);
  digitalWrite(in_2, LOW);
  Serial.println("Pompa 1 basa mati");
}

void startPump(int type, int speed, unsigned long duration) {
  pumpType = type;
  pumpSpeed = speed;
  pumpDuration = duration;
  previousMillis = millis();
  isPumping = true;

  if (pumpType == 1) {
    pump_1(pumpSpeed);
  } else if (pumpType == 2) {
    pump_2(pumpSpeed);
  }
}

void stopPump() {
  if (pumpType == 1) {
    stopMotor_1();
  } else if (pumpType == 2) {
    stopMotor_2();
  }
  isPumping = false;
  delay(5000);
}

void pump() {
  KalmanFilter();
  if (isPumping) {
    if (millis() - previousMillis >= pumpDuration) {
      stopPump();
    }
    return (KalmanFilter());
  }

  if (KalmanFilterData >= pHnetral_basa) {
    pH_target = KalmanFilterData - pHnetral_basa;
    if (pH_target >= 0.1 && pH_target <= 0.5) {
      Serial.println("pompa asam berjalan 1,965 detik");
      startPump(2, 50, 1965);
    } else if (pH_target >= 0.6 && pH_target <= 1) {
      Serial.println("pompa asam berjalan 2,231 detik");
      startPump(2, 50, 2231);
    } else if (pH_target >= 1.1 && pH_target <= 1.5) {
      Serial.println("pompa asam berjalan 2,497 detik");
      startPump(2, 50, 2497);
    } else if (pH_target >= 1.6 && pH_target <= 2) {
      Serial.println("pompa asam berjalan 2,763 detik");
      startPump(2, 50, 2763);
    } else if (pH_target >= 2.1 && pH_target <= 2.5) {
      Serial.println("pompa asam berjalan 3,028 detik");
      startPump(2, 50, 3028);
    } else if (pH_target >= 2.6 && pH_target <= 3) {
      Serial.println("pompa asam berjalan 3,294 detik");
      startPump(2, 50, 3294);
    } else if (pH_target >= 3.1 && pH_target <= 3.5) {
      Serial.println("pompa asam berjalan 3,560 detik");
      startPump(2, 50, 3560);
    } else if (pH_target >= 3.6 && pH_target <= 4) {
      Serial.println("pompa asam berjalan 3,826 detik");
      startPump(2, 50, 3826);
    } else if (pH_target >= 4.1 && pH_target <= 4.5) {
      Serial.println("pompa asam berjalan 4,091 detik");
      startPump(2, 50, 4091);
    } else if (pH_target >= 4.6 && pH_target <= 5) {
      Serial.println("pompa asam berjalan 4,357 detik");
      startPump(2, 50, 4357);
    } else if (pH_target >= 5.1 && pH_target <= 5.5) {
      Serial.println("pompa asam berjalan 4,623 detik");
      startPump(2, 50, 4623);
    } else if (pH_target >= 5.6 && pH_target <= 6) {
      Serial.println("pompa asam berjalan 4,889 detik");
      startPump(2, 50, 4889);
    } else if (pH_target >= 6.1 && pH_target <= 6.5) {
      Serial.println("pompa asam berjalan 5,154 detik");
      startPump(2, 50, 5154);
    } else if (pH_target >= 6.6 && pH_target <= 7) {
      Serial.println("pompa asam berjalan 5,420 detik");
      startPump(2, 50, 5420);
    } else if (pH_target >= 7.1 && pH_target <= 7.5) {
      Serial.println("pompa asam berjalan 5,686 detik");
      startPump(2, 50, 5686);
    }
  } else if (KalmanFilterData <= pHnetral_asam) {
    pH_target = pHnetral_asam - KalmanFilterData;
    if (pH_target >= 0.1 && pH_target <= 0.5) {
      Serial.println("pompa basa berjalan 2,731 detik");
      startPump(1, 50, 2731);
    } else if (pH_target >= 0.6 && pH_target <= 1) {
      Serial.println("pompa basa berjalan 3,462 detik");
      startPump(1, 50, 3462);
    } else if (pH_target >= 1.1 && pH_target <= 1.5) {
      Serial.println("pompa basa berjalan 4,193 detik");
      startPump(1, 50, 4193);
    } else if (pH_target >= 1.6 && pH_target <= 2) {
      Serial.println("pompa basa berjalan 4,924 detik");
      startPump(1, 50, 4924);
    } else if (pH_target >= 2.1 && pH_target <= 2.5) {
      Serial.println("pompa basa berjalan 5,655 detik");
      startPump(1, 50, 5655);
    } else if (pH_target >= 2.6 && pH_target <= 3) {
      Serial.println("pompa basa berjalan 6,386 detik");
      startPump(1, 50, 6386);  //
    } else if (pH_target >= 2.6 && pH_target <= 3) {
      Serial.println("pompa basa berjalan 7,117 detik");
      startPump(1, 50, 7117);
    } else if (pH_target >= 2.6 && pH_target <= 3) {
      Serial.println("pompa basa berjalan 7,848 detik");
      startPump(1, 50, 7848);
    } else if (pH_target >= 2.6 && pH_target <= 3) {
      Serial.println("pompa basa berjalan 8,579 detik");
      startPump(1, 50, 8579);
    }
  }
}

void loop() {
  KalmanFilter();
  // pump();
}