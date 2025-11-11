#ifndef ISENSOR_H
#define ISENSOR_H

class ISensor {
public:
    virtual ~ISensor() {}

    // Inicialización del sensor
    virtual bool init() = 0;

    // Verificar si hay datos disponibles para leer
    virtual bool dataReady() = 0;

    // Leer datos del sensor
    virtual bool read() = 0;

    // Getters para valores medidos
    virtual float getTemperature() = 0;  // Celsius
    virtual float getHumidity() = 0;     // Percentage 0-100
    virtual float getCO2() = 0;          // ppm (-1 si no aplica)

    // Nombre del tipo de sensor
    virtual const char* getSensorType() = 0;

    // Métodos opcionales
    virtual bool calibrate(float reference = 0) { return false; }
    virtual bool isActive() = 0;
};

#endif // ISENSOR_H
