#include <Tonttulib.h>

Tonttulib tonttu;

void setup() {
    Serial.begin(115200);

    int status = tonttu.init();
    if (status != 1) {
        Serial.print("Initialization failed with code: ");
        Serial.println(status);
        while(true);
    }

    Serial.println("All sensors initialized!");
}

void loop() {
    // --- BMP388 barometer ---
    float pressure = tonttu.baro.readPressure();
    Serial.print("Pressure: "); Serial.print(pressure); Serial.println(" Pa");

    // --- IMU ---
    float ax, ay, az;
    float gx, gy, gz;
    tonttu.imu.readAccel(ax, ay, az);
    tonttu.imu.readGyro(gx, gy, gz);

    Serial.print("Accel X: "); Serial.print(ax); Serial.print(" g, ");
    Serial.print("Y: "); Serial.print(ay); Serial.print(" g, ");
    Serial.print("Z: "); Serial.println(az);

    Serial.print("Gyro X: "); Serial.print(gx); Serial.print(" deg/s, ");
    Serial.print("Y: "); Serial.print(gy); Serial.print(" deg/s, ");
    Serial.print("Z: "); Serial.println(gz);

    // --- Flash ---
    uint8_t buffer[256];
    if (tonttu.flash.readPage(0, buffer)) {
        Serial.print("First byte of flash page 0: 0x");
        Serial.println(buffer[0], HEX);
    }

    Serial.println("-----------------------");
    delay(1000);
}
