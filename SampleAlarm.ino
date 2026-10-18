// GPI0 13 Asignar a la lectura del sensor infrarrojo
const int PIN_IRS = 13; 

// Pines del Sensor Supersonico
const int PIN_TRIG = 12; // Pin de disparo del sensor ultrasónico GPIO 12
const int PIN_ECHO = 14;    // Pin de eco del sensor ultrasónico GPIO 14

// Pin del botón para armar/desarmar el sistema
const int PIN_BUTTON = 4;  // Pin del botón GPIO 4 (Push Button con INPUT_PULLUP)

// Pin del LED integrado en la placa
const int PIN_LED    = 2;  // GPIO 2  (LED Azul Integrado en la placa)

// Pin sensor de vibración
const int PIN_VIBRATION = 34; // Pin del sensor de vibración SW-420 GPIO 34

// Pin sensor de proximidad
const int PIN_PROXIMITY = 27; // Pin del sensor de proximidad GPIO

// VARIABLES DE CONTROL DE ESTADO
bool systemArmed = true;
bool alarmTriggered = false;

long measureDistance() {
    digitalWrite(PIN_TRIG, LOW);
    delayMicroseconds(2);
    digitalWrite(PIN_TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(PIN_TRIG, LOW);

    // Mide la duración del pulso en el pin ECHO (timeout de 30ms)
    long duration = pulseIn(PIN_ECHO, HIGH, 30000); 

    // Si el sensor no recibe eco, devuelve 999 para evitar falsas alarmas
    if (duration == 0) return 999; 

    // Convierte el tiempo a distancia en centímetros (velocidad del sonido)
    return duration * 0.034 / 2;
}


void setup() {
    Serial.begin(115200);

    // Asignar el pin del sensor infrarrojo como entrada
    pinMode(PIN_IRS, INPUT);
    

    // Asignar los pines del sensor ultrasónico como entrada y salida
    pinMode(PIN_TRIG, OUTPUT);
    pinMode(PIN_ECHO, INPUT);

    // Asignar el pin del sensor de vibración como entrada
    pinMode(PIN_VIBRATION, INPUT);

    // Asignar el pin del botón como entrada con resistencia pull-up interna
    pinMode(PIN_BUTTON, INPUT_PULLUP);

    // Asignar el pin del sensor de proximidad como entrada con pull-up interna
    pinMode(PIN_PROXIMITY, INPUT_PULLUP);

    // Asignar el pin del LED como salida
    pinMode(PIN_LED, OUTPUT);
    digitalWrite(PIN_LED, LOW); // Asegurarse de que el LED esté apagado al inicio
  
    Serial.println("=========================================");
    Serial.println("--- Sistema de alarma inicializado ---");
    Serial.println("=========================================");
}

void loop() {

    // 1. Leer el estado del botón para armar/desarmar el sistema
    if (digitalRead(PIN_BUTTON) == LOW) { // Si el botón está presionado (estado bajo)
        delay(250); // Pequeño retraso para evitar rebotes
        systemArmed = !systemArmed; // Cambiar el estado del sistema
        alarmTriggered = false; // Reiniciar el estado de la alarma

        Serial.println("-----------------------------------------");
        Serial.print(">>> Botón presionado. Estado del sistema: ");
        Serial.println(systemArmed ? "ARMADO" : "DESARMADO");
        Serial.println("-----------------------------------------");
        // Mantener hasta que se suelte el botón
        while (digitalRead(PIN_BUTTON) == LOW);

    }

    // 2. Lógica de monitoreo del sistema cuando esta armado
    if (systemArmed) {
        
        int IRValue = digitalRead(PIN_IRS); // Leer el estado del sensor infrarrojo
        long distance = measureDistance(); // Medir la distancia con el sensor ultrasónico
        int vibrationValue = digitalRead(PIN_VIBRATION); // Leer el estado del sensor de vibración
        int proxValue = digitalRead(PIN_PROXIMITY); // Leer el estado del sensor de proximidad 

        //Condición de disparo: Infrarrojo detecta objeto (LOW) o distancia menor a 20 cm
        if (IRValue == LOW) {
            if (!alarmTriggered) {
                Serial.println(">>> Alarma activada por el sensor infrarrojo.");
                alarmTriggered = true;
                // digitalWrite(PIN_LED, HIGH); // Encender el LED de alarma
            }
        } else if (distance < 20) {
            if (!alarmTriggered) {
                Serial.println(">>> Alarma activada por el sensor ultrasónico.");
                alarmTriggered = true;
                // digitalWrite(PIN_LED, HIGH); // Encender el LED de alarma
            }
        } else if (vibrationValue == HIGH) {
            if (!alarmTriggered) {
                Serial.println(">>> Alarma activada por el sensor de vibración.");
                alarmTriggered = true;
                // digitalWrite(PIN_LED, HIGH); // Encender el LED de alarma
            }
        } else if (proxValue == LOW) {
            if (!alarmTriggered) {
                Serial.println(">>> Alarma activada por el sensor de proximidad.");
                alarmTriggered = true;
                // digitalWrite(PIN_LED, HIGH); // Encender el LED de alarma
            }
        }
        // Respuesta visual cuando se dispara la alarma
        if (alarmTriggered) {
            digitalWrite(PIN_LED, !digitalRead(PIN_LED)); // Parpadeo rápido del LED
            delay(100);
        } else {
            digitalWrite(PIN_LED, HIGH); // LED encendido fijo = Monitoreando
            delay(200);
        }
    } else {
        digitalWrite(PIN_LED, LOW); // LED apagado = Sistema desarmado
        delay(200);
    } 

    
}
