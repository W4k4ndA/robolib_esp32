# 🤖 Robolib — Librería Educativa de Robótica para ESP32

> **Aprende robótica real programando el microcontrolador directamente.**  
> Sin cajas negras, sin hardware propietario, sin límites.

---

## 🎯 ¿Qué es Robolib?

**Robolib** es una librería C++ moderna para **ESP32 (Arduino Core)** diseñada desde cero para **enseñar y aprender robótica** programando el hardware real.

| ❌ **Plataformas cerradas (LEGO, mBot, etc.)** | ✅ **Robolib** |
|-----------------------------------------------|----------------|
| Hardware propietario y caro | **Cualquier sensor/motor Arduino** ($1–$5) |
| Cajas negras: "funciona pero no sabes por qué" | **Código abierto**: ves y entiendes cada línea |
| Bloques visuales limitados | **C++ real** con patrones de diseño profesionales |
| Un solo robot predefinido | **Diseñas TU robot** componente a componente |
| Dependencia del fabricante | **Independencia total**: estándar Arduino/ESP32 |

---

## 🧠 Filosofía Educativa

| Principio | Cómo se aplica en Robolib |
|-----------|---------------------------|
| **Transparencia total** | Cada driver muestra cómo configurar pines, PWM, I2C, ADC |
| **Buenas prácticas C++** | RAII, Rule of Five, polimorfismo, templates, `const`-correctness |
| **Patrones de diseño** | Builder, Composite, Strategy, Dependency Injection |
| **Pensamiento abstracto** | Interfaces (`IDriverMotor`, `IDriverDistanceSensor`) desacoplan lógica de hardware |
| **Extensibilidad nativa** | Agregar un nuevo sensor = 1 clase que hereda de una interfaz |

> 💡 **Objetivo**: Que el estudiante entienda **qué hace el microcontrolador**, no solo "qué bloque arrastrar".

---

## 🏗️ Arquitectura

```
┌─────────────────────────────────────────────────────────────┐
│                        Robot (Composite)                     │
│  ┌─────────────────────┐  ┌─────────────────────────────┐   │
│  │  motors (Service)   │  │  distance (Service)         │   │
│  │  HBridgeMotorService│  │  DistanceService            │   │
│  └──────────┬──────────┘  └──────────────┬──────────────┘   │
│             │                            │                   │
│             ▼                            ▼                   │
│  ┌─────────────────────────────────────────────────────────┐ │
│  │              IRobotModule (Interface común)              │ │
│  │  virtual void init() = 0   |   virtual void update() {} │ │
│  └─────────────────────────────────────────────────────────┘ │
│             ▲                            ▲                   │
│             │                            │                   │
│  ┌──────────┴──────────┐    ┌───────────┴────────────┐      │
│  │IDriverHBMotor       │    │IDriverDistanceSensor   │      │
│  │(Interface)          │    │(Interface)             │      │
│  └──────────┬──────────┘    └───────────┬────────────┘      │
│             │                            │                   │
│  ┌──────────┴──────────┐    ┌───────────┴────────────┐      │
│  │HBridgeMotorDriver   │    │HCSR04SensorDriver      │      │
│  │(Implementa)         │    │(Implementa)            │      │
│  └─────────────────────┘    └────────────────────────┘      │
└─────────────────────────────────────────────────────────────┘
                              ▲
                              │ build()
                              │
┌─────────────────────────────────────────────────────────────┐
│                      RobotBuilder (Builder)                  │
│  .addModule(motor).addModule(sensor).build()                │
└─────────────────────────────────────────────────────────────┘
```

> **Nota**: Los servicios (`motors`, `distance`) son **fachadas especializadas** que encapsulan la lógica de cada dominio. `Robot` solo gestiona ciclo de vida (`begin()`, `update()`) y ownership.

---

## 🚀 Inicio Rápido

### 1. Instalación (PlatformIO)

```ini
; platformio.ini
[env:esp32-c3-devkitm-1]
platform = espressif32
board = esp32-c3-devkitm-1
framework = arduino
lib_deps = 
    ; Robolib se incluye como librería local en lib/Robolib
```

Estructura esperada:
```
tu_proyecto/
├── lib/
│   └── Robolib/          ← Clona o copia aquí la librería
│       ├── Robolib.h
│       ├── library.json
│       └── src/...
├── src/
│   └── main.cpp
└── platformio.ini
```

### 2. Tu primer robot

```cpp
#include <Arduino.h>
#include <Robolib.h>

using namespace robolib;

// Pines para TB6612FNG (2 motores DC)
#define PIN_AIN1 0   #define PIN_AIN2 1
#define PIN_BIN1 2   #define PIN_BIN2 3
#define CH_AIN1  0   #define CH_AIN2  1
#define CH_BIN1  2   #define CH_BIN2  3

// 1. Construir el robot  (Builder Pattern)
auto robot = RobotBuilder()
    .addModule(new HBridgeMotorDriver(PIN_AIN1, PIN_AIN2, CH_AIN1, CH_AIN2))  // Motor izquierdo
    .addModule(new HBridgeMotorDriver(PIN_BIN1, PIN_BIN2, CH_BIN1, CH_BIN2))  // Motor derecho
    .addModule(new HCSR04SensorDriver(4, 5))   // Sensor ultrasónico (Trig, Echo)
    .addModule(new TCRT5000SensorDriver(6))    // Sensor de línea (pin ADC)
    .build();

void setup() {
    Serial.begin(115200);


    // 2. Inicializar TODO el hardware (pines, PWM, I2C, etc.)
    robot->begin();

}


void loop() {
    if (!robot) return;

    // Control diferencial vía servicio de motores
    robot->motors.moveDifferential(200, 200);  // Adelante
    delay(1000);

    robot->motors.moveDifferential(-150, 150); // Giro sobre eje
    delay(500);

    // Leer sensor de distancia vía servicio
    float dist = robot->distance.getCM(0);
    Serial.printf("Distancia: %.1f cm\n", dist);

    delay(100);
}
```

---

## 📦 Componentes Incluidos

### 🎮 Actuadores

| Driver | Hardware soportado | Interfaz | Servicio |
|--------|-------------------|----------|----------|
| `HBridgeMotorDriver` | L298N, TB6612FNG, BTS7960, cualquier puente H 2 canales | `IDriverHBMotor` | `robot->motors` |

### 📏 Sensores de Distancia

| Driver | Hardware | Interfaz | Servicio |
|--------|----------|----------|----------|
| `HCSR04SensorDriver` | HC-SR04 (ultrasónico) | `IDriverDistanceSensor` | `robot->distance` |

### 🛤️ Sensores de Línea

| Driver | Hardware | Interfaz | Servicio |
|--------|----------|----------|----------|
| `TCRT5kSensorDriver` | TCRT5000 (IR reflectivo) | `IDriverDigitalLineSensor` |`robot->digitalLine` |


---

## 🛠️ API Principal — Arquitectura Basada en Servicios

`Robot` expone **servicios públicos** que encapsulan la lógica por dominio:

```cpp
class Robot {
public:
    DistanceService     distance{modules};  // 📏 Sensores de distancia
    HBridgeMotorService motors{modules};    // 🎮 Motores puente H
    
    void begin();      // Inicializa TODOS los módulos (llama init() en cada uno)
    void update();     // Llama update() en cada módulo (PID, filtrado, etc.)
};
```

### 📏 `robot->distance` — DistanceService

| Método | Descripción |
|--------|-------------|
| `getCM(index = 0)` | Distancia en cm (negativo = error) |
| `count()` | Número de sensores registrados |
| `exists()` | `true` si hay al menos uno |

```cpp
float d = robot->distance->getCM(0);      // Primer sensor
float d2 = robot->distance->getCM(1);     // Segundo sensor
if (robot->distance->exists()) { ... }
```

### 🎮 `robot->motors` — HBridgeMotorService

| Método | Descripción |
|--------|-------------|
| `moveMotor(index, speed)` | Velocidad -255 a 255 |
| `stopMotor(index, breaked)` | `breaked=true` = freno activo |
| `stopAll(breaked)` | Detiene todos |
| `moveDifferential(left, right)` | Control tracción diferencial (índices 0 y 1) |
| `count()` | Número de motores |
| `exists()` | `true` si hay al menos uno |

```cpp
robot->motors->moveDifferential(200, 200);   // Adelante
robot->motors->moveMotor(0, -150);           // Motor 0 reversa
robot->motors->stopAll(true);                // Freno activo en todos
```

### 🔄 Ciclo de vida

```cpp
robot->begin();   // setup(): inicializa pines, PWM, I2C, calibra sensores
robot->update();  // loop(): actualiza PID, filtros, máquinas de estado
```

---

## 🧩 Cómo Extender: Agregar TU Propio Hardware

### Paso 1: Elige la interfaz base

```cpp
// Para un NUEVO tipo de motor (puente H)
class IDriverHBMotor : public IRobotModule {
    virtual void move(int16_t speed) = 0;
    virtual void stop(bool stacked = false) = 0;
};

// Para un NUEVO sensor de distancia
class IDriverDistanceSensor : public IRobotModule {
    virtual float measureCM() = 0;
};

// Para un NUEVO sensor de línea
class IDriverLineSensor : public IRobotModule {
    virtual bool isOnLine() = 0;
    virtual int getAnalogRawValue() = 0;
    virtual void setThreshold(int) = 0;
    // ... setOnLineIs0/1
};
```

### Paso 2: Implementa tu driver

```cpp
// MiNuevoSensor.h
#include "Interfaces/IDriverDistanceSensor.h"

class MiNuevoSensor : public IDriverDistanceSensor {
    uint8_t _pinSDA, _pinSCL;
public:
    MiNuevoSensor(uint8_t sda, uint8_t scl) : _pinSDA(sda), _pinSCL(scl) {}

    void init() override {
        Wire.begin(_pinSDA, _pinSCL);
        // Configurar registros del sensor...
    }

    float measureCM() override {
        // Leer sensor via I2C, convertir a cm
        return distancia_cm;
    }

    void update() override {
        // Opcional: filtrado, calibración continua, etc.
    }
};
```

### Paso 3: Úsalo INMEDIATAMENTE

```cpp
auto robot = RobotBuilder()
    .addModule(new MiNuevoSensor(21, 22))  // ¡Ya funciona!
    .addModule(new HBridgeMotorDriver(...))
    .build();

robot->begin();
float d = robot->distance.getCM(0);  // Polimorfismo en acción
```

> ✨ **Cero modificaciones a Robolib**. Solo tu clase nueva.

---

## 🎓 Conceptos C++ que Aprendes Usando Robolib

| Concepto | Dónde se ve en Robolib |
|----------|------------------------|
| **Polimorfismo** | `IRobotModule*` apunta a `HBridgeMotorDriver`, `HCSR04SensorDriver`, etc. |
| **Interfaces (clases abstractas)** | `IDriverHBMotor`, `IDriverDistanceSensor` con `= 0` |
| **Builder Pattern** | `RobotBuilder::addModule().build()` |
| **Composite Pattern** | `Robot` trata motores y sensores uniformemente vía `IRobotModule*` |
| **Façade Pattern** | `DistanceService`, `HBridgeMotorService` simplifican API compleja |
| **Dependency Injection** | `RobotBuilder` inyecta drivers en `Robot` → servicios |
| **RAII / Ownership** | `Robot` destruye drivers en destructor (`delete`) |
| **Templates** | `getModule<T>()` y `countModules<T>()` en `ModuleHelper` |
| **`const`-correctness** | `getCM() const`, `exists() const`, `count() const` |
| **`dynamic_cast`** | Filtrado seguro de tipos en tiempo de ejecución (`ModuleHelper`) |
| **Rule of Five** | `Robot` tiene destructor, copy/move = delete |
| **`static_cast`** | Conversiones seguras `int16_t` → `uint32_t` para PWM |
| **`auto` + type deduction** | `auto* motor = getModule<IDriverHBMotor>(0)` |
| **Herencia de constructores** | `using ModuleHelper::ModuleHelper` en servicios |

---

## 📁 Estructura de Archivos

```
lib/Robolib/
├── Robolib.h              # Header principal (incluye todo)
├── library.json           # Metadatos PlatformIO
├── README.md              # Este archivo
└── src/
    ├── Robot.h/.cpp       # Núcleo: Robot + RobotBuilder
    ├── Interfaces/
    │   ├── IRobotModule.h         # Interface base (init, update)
    │   ├── IDriverHBMotor.h       # Interface motores puente H
    │   ├── IDriverDistanceSensor.h
    │   └── IDriverLineSensor.h
    ├── Controllers/
    │   ├── Actuators/Motors/
    │   │   └── HBridgeMotorDriver.h/.cpp
    │   ├── Sensors/Distance/
    │   │   └── HCSR04SensorDriver.h/.cpp
    │   └── Sensors/Line/
    │       └── TCRT5000SensorDriver.h/.cpp
    └── Services/
        ├── DistanceService.h      # Fachada sensores distancia → robot->distance
        ├── HBridgeMotorService.h  # Fachada motores puente H → robot->motors
        └── Helper.h               # Utilidades (templates getModule, countModules)
```

---

## 🧪 Ejemplos de Proyectos

| Proyecto | Componentes | Servicios usados | Conceptos |
|----------|-------------|------------------|-----------|
| **Evitador de obstáculos** | HC-SR04 + 2× Motor DC | `distance`, `motors` | Lógica reactiva, histéresis |
| **Seguidor de línea** *(próximamente)* | 2× TCRT5000 + 2× Motor DC | `line`, `motors` | PID, máquina de estados |
| **Mini Sumo** | 2× Motor + 2× TCRT5000 (borde) + HC-SR04 (oponente) | `motors`, `distance`, `line` | FSM compleja, timers |
| **Robot explorador** | Motor + Servo + HC-SR04 montado en servo | `motors`, `distance` | Mapeo 2D, planificación |

---

## 🤝 Contribuir

1. **Fork** → Crea tu driver nuevo en `src/Controllers/...`
2. **Sigue el estilo**: `init()`, `update()`, `virtual ~() = default`
3. **Documenta** con Doxygen (`/// @brief`)
4. **PR** con ejemplo de uso


---

## 🙌 Agradecimientos

- **Espressif** por el ESP32 y Arduino Core
- **Comunidad Arduino** por sensores/motores accesibles
- **Estudiantes y docentes** que prueban, rompen y mejoran esto

---

> **¿Dudas? ¿Ideas? ¿Nuevo hardware?**  
> Abre un *Issue* o *Discussion* en el repo.  
> **Robolib crece con la comunidad.** 🌱