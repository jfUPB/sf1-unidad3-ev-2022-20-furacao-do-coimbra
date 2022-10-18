# Evaluación unidad #3 
## Equipo: *Furacao do coimbra*

### Participantes: 

* William Alejandro Pabón - ID: 225366
* Juan Pablo Correa - ID: 449689
* Manuela Cuervo - ID: 446641
* Yeison Andrés Muñoz - ID: 456968

## Enunciado

"Considera el siguiente fragmento de este video que va desde el segundo 16 hasta el 34. 
También puede usar como referencia este otro video. Tu misión será reproducir lo que ves allí bajo estas restricciones:

1. Usa Unity.
2. Usa el sensor ADXL345 o MPU-6050

![MPU6050 and ESP32](https://content.instructables.com/ORIG/F1U/RBUZ/KFDYWZIZ/F1URBUZKFDYWZIZ.png?auto=webp&frame=1&fit=bounds&md=773d9ab9ee3bf6f0f90e33771377beb5)

3. Debes usar un protocolo de comunicación BINARIO.

* Entregar: El código fuente de las aplicaciones para el microcontrolador y para Unity en este repositorio.
Documentar en el archivo README.md cómo funciona el proceso de integración de los valores que entrega el sensor con el
contenido digital de la aplicación interactiva.

### Referencias

* How To Track Orientation with Arduino | ADXL345 Accelerometer Tutorial
> https://www.youtube.com/watch?v=KMhbV1p3MWk&t=16s

* MPU-6050 Six-Axis (Gyro + Accelerometer) unity 3d and Arduino
> https://www.youtube.com/watch?v=L4WfHT_58Dg

#
## Review del código (Microcontroller)


* Aquí hacemos lo más escencial que es inicializar las librerías y dependencias necesarias para poder hacer la comunicación del programa y el micro-controlador posible. Después procedemos a referenciar y declarar las variables que usaremos en nuestro código.

* Después de esto se inicializa la función *Void setup* la cual se encarga de ejecutarse cada vez que inicia el programa, así buscando el puerto serial correspondiente e iniciandolo y buscando la cominicación con el giroscopio.

```cpp
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;
float roll, pitch,yaw;
float x, y, z;
float gx, gy, gz;

void setup(void) {
  
  Serial.begin(115200);
  while (!Serial)
    delay(10);
  Serial.println("Adafruit MPU6050 test!");

  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }

  Serial.println("MPU6050 Found!");
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: RANGE_8_G");
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: RANGE_500_DEG");
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
  Serial.print("Filter bandwidth set to: BAND_5_HZ");
}

```

* Posteriormente está la clase *Void Loop*, la cual está encargada de ejecutarse cada frame para poder tener cierto flujo de datos que nos envían unas ecuaciones hechas para calcular el movimiento de los ejes del acelerómetro y estos se les asignas a unas variables que usaremos más tarde. Lo que ejecuta después de esto nos permite verificar si hay algún puerto serial con datos disponibles, por lo que después revisa si ese dato es igual a *S* debido a que escogimos este mensaje para comunicar el Micro-Controlador con la aplicación *UNITY*.

```cpp

void loop() {
  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;

  mpu.getEvent(&a, &g, &temp);

  roll = 180 *atan(a.acceleration.x/sqrt(a.acceleration.y*a.acceleration.y+a.acceleration.z*a.acceleration.z))/M_PI;
  pitch = 180 *atan(a.acceleration.y/sqrt(a.acceleration.x*a.acceleration.x+a.acceleration.z*a.acceleration.z))/M_PI;
  //Calcule with yaw
  yaw = 180 *atan(a.acceleration.z/sqrt(a.acceleration.y*a.acceleration.y+a.acceleration.z*a.acceleration.z))/M_PI;
  yaw = yaw - 45;
  //Z acceleration
  z = a.acceleration.z;
  //Z gyro
  gz = g.gyro.z;

  if(Serial.available() > 0)
  {
    if(Serial.read() == 's')
    {
      Serial.write((u_int8_t*)&roll,4);
      Serial.write((u_int8_t*)&pitch,4);
      //Rotation in z axis cannot be calcule with Acceleration values
      //Program would be correct to use Quaternions in the near future to solve this issue
      Serial.write((u_int8_t*)&gz,4);
    }        
  }    
}

```

## Review del código (Unity)

* Aquí hacemos algo muy similar al código del micro-controlador, inicializamos librerías, declaramos varaibles que usaremos para el programa y abrimos las comunicaciones del puerto serial, de esta manera la aplicación podrá compartirse mensajes con el micro-controlador.

```cpp
using System;
using System.Collections;
using System.Collections.Generic;
using System.IO.Ports;
using UnityEngine;

public class Control : MonoBehaviour
{
    SerialPort _serialPort;
    float nx, ny, nz;
    byte[] buffer = new byte[12];
    
    void Start()
    {
        _serialPort = new SerialPort();
        _serialPort.PortName = "/dev/ttyUSB0";
        _serialPort.BaudRate = 115200;
        _serialPort.DtrEnable = true;
        _serialPort.Open();
        Debug.Log("Open Serial Port");
    }
```

* El método *void Update* se ejecuta una vez por frame, el cual se comunica constantemente mandando el mensaje que definimos en el código anterior, confirmando el envío y la recepción de datos, este lee la cadena de 12 bites que conformaría los 3 datos que necesitamos para poder hacer posibles las rotaciones básicas, y por medio del buffer, se dividen estos bites en paquetes de 4, los cuales se leen en ese orden y se tranforman en un entero para darle la transformación a un *GameObject* en la aplicación.

```cpp

 // Update is called once per frame
    void Update()
    {
        _serialPort.Write("s");
        
        if (_serialPort.BytesToRead >= 12)
        {
            Debug.Log("rec");
            _serialPort.Read(buffer, 0, 12);
            nz = BitConverter.ToSingle(buffer, 0);
            nx = - BitConverter.ToSingle(buffer, 4);
            ny = BitConverter.ToSingle(buffer, 8);
        }
        transform.rotation = Quaternion.Euler(nx,ny,nz);
        
    }
}

```
