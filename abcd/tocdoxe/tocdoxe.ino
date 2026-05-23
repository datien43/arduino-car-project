#include <IRremote.h>

// ==========================
// update branch
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
// BIẾN HỆ THỐNG
// ==========================
bool heThongSanSang = false;

const int KHOANG_CACH_VAT_CAN = 20;

// Tốc độ xe
int tocDoTien = 70;     // giảm số này để xe chậm hơn
int tocDoRe = 190;

void setup() {

  Serial.begin(9600);

  // Motor
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  // HY-SRF05
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // IR
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

void loop() {

  // ==========================
  // ĐỌC REMOTE
  // ==========================
  if (IrReceiver.decode()) {

    int code = IrReceiver.decodedIRData.command;

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

  delay(60);
}