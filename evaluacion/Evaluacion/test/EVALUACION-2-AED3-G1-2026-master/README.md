# 🎃 EVALUACIÓN DIGITALES 3: HALLOWEEN 👻

Buenas tardes queridos alumnos. Hoy se encuentran ante la última (esperemos) evaluación de digitales 3. Este fin de semana que pasó fue Halloween y como para no quedarnos atrás esta evaluación tendrá la temática correspondiente.

Esta vez su tarea no será cazar fantasmas, escapar de zombies ni sobrevivir a una habitación embrujada...

Su misión será algo mucho más terrorífico:

**Crear una librería para el STM32.** 👻

El protagonista de esta historia será el **DS1302**, un circuito integrado capaz de mantener información de hora y calendario. RTC para los amigos.

Pero hay un pequeño problema...

No se comunica mediante UART, tampoco mediante I2C y tampoco utiliza SPI de manera estándar.

El DS1302 posee su propio protocolo serie síncrono de **3 hilos**, por lo que deberán interpretar su hoja de datos y programar la comunicación desde cero.

A continuación se sumergirán en la aventura...  muajajaja

---

## 🧞‍♂️ El Desafío: Crear el Driver

Su misión es desarrollar una librería en C para el **STM32F103C8** que permita comunicarse correctamente con un **DS1302**.

La librería deberá ser capaz de:

- Inicializar los GPIO necesarios.
- Escribir registros del DS1302.
- Leer registros del DS1302.
- Configurar una hora determinada.
- Obtener horas, minutos y segundos.
- Interpretar correctamente el formato en el que el RTC almacena la información.

La lógica de comunicación deberá desarrollarse a partir de la **hoja de datos del DS1302**.

---

## 🧛‍♂️ El Protocolo de las Tinieblas

El DS1302 utiliza solamente tres líneas para comunicarse con el STM32:

- **CE:** habilita una transferencia.
- **SCLK:** señal de clock de la comunicación.
- **I/O:** línea por donde se transmiten y reciben los datos.

El STM32 es el encargado de controlar estas señales mediante GPIO.

### 🕯️ Una pista importante

`SCLK` no es un clock automático generado mágicamente por el DS1302.

**Ustedes deberán generarlo mediante software.**

Eso significa que tendrán que controlar sus flancos mediante el GPIO correspondiente.

Pero recuerden algo de comunicaciones síncronas:

> El clock no necesariamente genera el dato. El clock indica en qué momento ese dato debe ser interpretado.

Presten especial atención en la hoja de datos a los momentos donde se recibe o donde se entrega la información.

Un error de un solo flanco puede hacer que toda la comunicación quede desplazada.

👻 **Ya fueron avisados.**

---

## 🧟 El Pin I/O Tiene Dos Personalidades

A diferencia de otros protocolos que utilizan una línea para transmitir y otra para recibir, el DS1302 utiliza **un único pin I/O para ambas cosas**.

Por lo tanto, durante una comunicación deberán decidir cuándo el STM32:

```text
ENVÍA datos
```

y cuándo:

```text
RECIBE datos
```

Eso implica modificar la configuración del GPIO entre:

```text
SALIDA
```

y:

```text
ENTRADA
```

según corresponda.

Recuerden cómo se configuran los GPIO del STM32F103 utilizando los registros:


La biblia del STM32 estará disponible en este repositorio para aquellos que necesiten invocar los conocimientos ancestrales. 📖

---

## 🧙‍♂️ El Byte de Comando

Cada comunicación con el DS1302 comienza enviando un **byte de comando**.

Este byte le indica al dispositivo:

- Qué registro se quiere utilizar.
- Si se quiere acceder al reloj/calendario o a la RAM.
- Si la operación será de lectura o escritura.

Presten especial atención al orden en el que deben transmitirse los bits.

No siempre los protocolos envían primero el bit que uno intuitivamente esperaría.

🧠 **Lean el datasheet.**

La respuesta está ahí.

---

## 🕸️ Escritura y Lectura

Una operación de escritura tendrá conceptualmente la siguiente estructura:

```text
Inicio de comunicación
        ↓
Byte de comando
        ↓
Byte de datos
        ↓
Fin de comunicación
```

En una lectura ocurre algo diferente:

```text
Inicio de comunicación
        ↓
Byte de comando
        ↓
Cambio de dirección del pin I/O
        ↓
Recepción del byte
        ↓
Fin de comunicación
```

Hay un pequeño detalle entre el **último bit del comando de lectura** y el **primer bit que devuelve el DS1302**.

Ese detalle está relacionado con los flancos de `SCLK`.

Si no lo respetan, probablemente pierdan el primer bit de la lectura y todo lo demás quedará corrido.

💀 **Checkeen cuidadosamente los diagramas temporales, por ahi esta la magia.**

---

## 🧛‍♀️ Los Registros del Reloj

El DS1302 posee registros independientes para almacenar:

- Segundos.
- Minutos.
- Horas.
- Fecha.
- Mes.
- Día.
- Año.

Para esta evaluación nos concentraremos principalmente en:

```text
SEGUNDOS
MINUTOS
HORAS
```

Cada registro posee una dirección diferente para:

```text
LECTURA
```

y:

```text
ESCRITURA
```

Estas direcciones deberán definirse en su archivo:

```text
ds1302.h
```

mediante `#define`.

---

## 🔮 El Misterio del BCD

El DS1302 no almacena directamente la hora como un número binario normal.

Utiliza un formato llamado:

**BCD — Binary Coded Decimal**

Por ejemplo, si tenemos:

```text
37 segundos
```

el DS1302 almacena:

```text
0011 0111
```

Es decir:

```text
0011 = 3
0111 = 7
```

Por lo tanto:

```text
37 decimal ≠ 37 binario dentro del registro
```

Deberán encontrar la forma de convertir:

```text
DECIMAL → BCD
```

cuando quieran configurar la hora,

y:

```text
BCD → DECIMAL
```

cuando quieran leerla.


---

## 💀 Dos Bits Que Pueden Arruinarles la Tarde

Hay dos bits del DS1302 a los que deberán prestar especial atención durante la inicialización.

### 👻 Write Protect

El DS1302 posee una protección contra escritura.

Si está activada, pueden mandar todos los bits que quieran...

El dispositivo simplemente ignorará sus intentos de modificar los registros.

Antes de escribir la hora deberán asegurarse de que la escritura esté habilitada.

### 🪦 Clock Halt

El registro de segundos posee un bit que permite detener el oscilador interno del RTC.

Si dicho bit queda activado:

```text
el reloj no avanzará.
```

Su librería deberá asegurarse de que el reloj quede funcionando correctamente.

Lean la tabla de registros del datasheet para identificar ambos bits.

---

## 🧪 Funciones a Implementar — Los Hechizos

La librería deberá contener como mínimo las siguientes funciones:

```c
void ds1302_init();
```

**Propósito:**  
Inicializar los GPIO necesarios y dejar al DS1302 preparado para funcionar.

---

```c
void ds1302_write(uint8_t address, uint8_t data);
```

**Propósito:**  
Escribir un byte en uno de los registros del DS1302.

---

```c
uint8_t ds1302_read(uint8_t address);
```

**Propósito:**  
Leer un byte desde uno de los registros del DS1302.

---

```c
void ds1302_set_time(uint8_t hora, uint8_t min, uint8_t seg);
```

**Propósito:**  
Configurar una hora determinada.

La función deberá encargarse de convertir los valores recibidos al formato utilizado por el DS1302.

---

```c
uint8_t ds1302_get_hours();
uint8_t ds1302_get_minutes();
uint8_t ds1302_get_seconds();
```

**Propósito:**  
Obtener individualmente las horas, minutos y segundos almacenados en el RTC.

Estas funciones deberán devolver valores decimales normales para que puedan ser utilizados fácilmente desde `main.c`.

---

## 🦇 Funciones Recomendadas — No Son Obligatorias, Pero Les Van a Servir

Además de las funciones obligatorias de la librería, hay algunas funciones auxiliares que **no son requisito** y no necesariamente tienen que llamarse exactamente así.

Sin embargo, separar ciertas tareas en funciones más pequeñas puede hacer que el código sea mucho más fácil de entender, probar y corregir.

Por ejemplo, podrían resultarles útiles funciones encargadas de:

```c
void io_output();
void io_input();

void sclk_high();
void sclk_low();

void ce_high();
void ce_low();
```

Estas funciones permitirían separar la lógica de configuración y control de los GPIO del funcionamiento general del protocolo.

⚠️ **IMPORTANTE**

Estas funciones:

- No son obligatorias.
- No tienen que tener necesariamente estos nombres.
- No tienen que implementar exactamente esta estructura.
- No forman parte de lo que deberá utilizar el `main.c`.

Son simplemente una recomendación para organizar mejor la librería.

La idea es que puedan dividir un problema grande:

```text
comunicarse con el DS1302
```

en problemas más pequeños:

```text
configurar I/O
        ↓
manejar CE
        ↓
manejar SCLK
        ↓
mandar bits
        ↓
recibir bits
        ↓
armar bytes
        ↓
leer/escribir registros
```

Mientras la librería funcione correctamente y respete el protocolo indicado por el datasheet, cada grupo puede decidir cómo organizar internamente su código.

A veces el mejor hechizo no es el más largo...

están avisados changuitos.


## 🧟 Estructura de la Librería

El proyecto deberá contener como mínimo:

```text
ds1302.h
ds1302.c
main.c
```

En:

```text
ds1302.h
```

deberán encontrarse:

- Los `#define` necesarios.
- Las direcciones de los registros utilizados.
- La configuración de pines y puerto.
- Los prototipos de las funciones públicas de la librería.

En:

```text
ds1302.c
```

deberá desarrollarse toda la lógica correspondiente al protocolo.

El `main.c` deberá utilizar la librería para demostrar que el RTC funciona correctamente.



---

## 👹 Forma de Entrega

Deberán sintetizar toda su magia en un repositorio de GitHub.

El repositorio deberá llamarse exactamente:

```text
APELLIDO_NOMBRE_EV_AED3G1
```

Deberán subir desde VSCode el **workspace completo del proyecto**, como vienen trabajando durante el año. Salvo los que están con notebooks, en ese caso me llaman y lo subimos juntos.

El repositorio deberá incluir correctamente:

```text
main.c
ds1302.c
ds1302.h
```

y todos los archivos necesarios para poder revisar el proyecto.

### ⚠️ ATENCIÓN

Si la forma de entrega, la estructura del proyecto o el nombre del repositorio no respetan lo solicitado, el alumno podrá ser penalizado con hasta:

**3 (tres) puntos menos.**

No quiero encontrar repositorios llamados:

```text
trabajo
prueba
stm
proyecto nuevo
ds1302 final ahora si
```

Y mucho menos alguna vulgaridad... 🎃

---

## 🍬 El Trato

No están completamente solos en esta casa embrujada.

Dentro del repositorio encontrarán:

- La hoja de datos del **DS1302**.
- Y una sorpresa...

Pero esta vez no habrá una librería que resuelva la comunicación.

Deberán interpretar:

- El protocolo.
- Los diagramas temporales.
- Los registros.
- Los flancos.
- El formato de los datos.

Y transformarlo en código.

La respuesta está en los manuales.

Solo tienen que encontrarla. 🔮

Ahora es su turno.

Hagan hablar al DS1302, mantengan vivo el reloj y traten de no perderse entre los bits...

🦇 **¡Feliz Halloween!** 🎃
