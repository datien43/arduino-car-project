#include <IRremote.h>

// ==========================
// IR RECEIVER
// ==========================
const int RECV_PIN = 11;

// ==========================
// L298N
// ==========================

// Motor A
const int ENA = 9;
const int IN1 = 2;
const int IN2 = 3;

// Motor B
const int ENB = 10;
const int IN3 = 4;
const int IN4 = 5;

// ==========================
// HY-SRF05
// ==========================
const int TRIG_PIN = 8;
const int ECHO_PIN = 7;

// ==========================
// XI NHAN
// ==========================
const int XI_NHAN_TRAI = 6;
const int XI_NHAN_PHAI = 12;

// ==========================
// BIẾN HỆ THỐNG
// ==========================
bool heThongSanSang = false;

bool xiNhanTrai = false;
bool xiNhanPhai = false;

unsigned long previousBlink = 0;
bool ledState = false;

const int KHOANG_CACH_VAT_CAN = 20;

// Tốc độ xe
int tocDoTien = 70;
int tocDoRe = 190;

void setup() {

  Serial.begin(9600);

  // ==========================
  // MOTOR
  // ==========================
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  // ==========================
  // HY-SRF05
  // ==========================
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // ==========================
  // XI NHAN
  // ==========================
  pinMode(XI_NHAN_TRAI, OUTPUT);
  pinMode(XI_NHAN_PHAI, OUTPUT);

  // ==========================
  // IR REMOTE
  // ==========================
  IrReceiver.begin(RECV_PIN, ENABLE_LED_FEEDBACK);

  Serial.println("He thong cho...");
}

// ==========================
// HÀM XE TIẾN
// ==========================
void xeTien() {

  analogWrite(ENA, tocDoTien);
  analogWrite(ENB, tocDoTien);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

// ==========================
// HÀM XE DỪNG
// ==========================
void xeDung() {

  analogWrite(ENA, 0);
  analogWrite(ENB, 0);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

// ==========================
// HÀM XE RẼ PHẢI
// ==========================
void xeRePhai() {

  analogWrite(ENA, tocDoRe);
  analogWrite(ENB, tocDoRe);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

// ==========================
// HÀM XI NHAN
// ==========================
void capNhatXiNhan() {

  if (millis() - previousBlink >= 500) {

    previousBlink = millis();

    ledState = !ledState;

    // Xi nhan trái
    if (xiNhanTrai) {
      digitalWrite(XI_NHAN_TRAI, ledState);
    }
    else {
      digitalWrite(XI_NHAN_TRAI, LOW);
    }

    // Xi nhan phải
    if (xiNhanPhai) {
      digitalWrite(XI_NHAN_PHAI, ledState);
    }
    else {
      digitalWrite(XI_NHAN_PHAI, LOW);
    }
  }
}

void loop() {

  // ==========================
  // ĐỌC REMOTE
  // ==========================
  if (IrReceiver.decode()) {

    int code = IrReceiver.decodedIRData.command;

    Serial.print("Ma nut: ");
    Serial.println(code);

    // ==========================
    // NÚT POWER
    // ==========================
    if (code == 69) {

      heThongSanSang = !heThongSanSang;

      if (heThongSanSang) {

        Serial.println("HE THONG BAT");
        xeTien();
      }
      else {

        Serial.println("HE THONG TAT");
        xeDung();
      }
    }

    // ==========================
    // XI NHAN TRÁI
    // ==========================
    if (code == 12) {

      xiNhanTrai = !xiNhanTrai;

      // Tắt xi nhan phải
      xiNhanPhai = false;

      Serial.println("XI NHAN TRAI");
    }

    // ==========================
    // XI NHAN PHẢI
    // ==========================
    if (code == 24) {

      xiNhanPhai = !xiNhanPhai;

      // Tắt xi nhan trái
      xiNhanTrai = false;

      Serial.println("XI NHAN PHAI");
    }

    IrReceiver.resume();
  }

  // ==========================
  // XE HOẠT ĐỘNG
  // ==========================
  if (heThongSanSang) {

    long duration;
    int distance;

    // Đo khoảng cách
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);

    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);

    digitalWrite(TRIG_PIN, LOW);

    duration = pulseIn(ECHO_PIN, HIGH, 30000);

    distance = int(duration / 2 / 29.412);

    Serial.println(distance);

    // Có vật cản
    if (distance > 0 && distance < KHOANG_CACH_VAT_CAN) {

      xeDung();

      delay(3000);

      while (true) {

        xeRePhai();

        // Đo lại
        digitalWrite(TRIG_PIN, LOW);
        delayMicroseconds(2);

        digitalWrite(TRIG_PIN, HIGH);
        delayMicroseconds(10);

        digitalWrite(TRIG_PIN, LOW);

        duration = pulseIn(ECHO_PIN, HIGH, 30000);

        distance = int(duration / 2 / 29.412);

        if (distance >= KHOANG_CACH_VAT_CAN || distance == 0) {

          break;
        }

        delay(50);
      }
    }

    // Không có vật cản
    else {

      xeTien();
    }
  }

  // ==========================
  // CẬP NHẬT XI NHAN
  // ==========================
  capNhatXiNhan();

  delay(60);
}