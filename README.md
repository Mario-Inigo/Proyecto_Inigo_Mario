El código implementa un programa que mide el peso a través de una galga extensiométrica y realiza diversas acciones según el estado del sistema. A continuación, se proporciona un comentario detallado sobre las partes claves del código:

- Se definen objetos para representar diferentes componentes del sistema, como botones, LEDs, la galga extensiométrica, la alarma y una pantalla LCD RGB.
- Se declaran variables para almacenar información crucial, como el peso, los voltajes medios en calibraciones, la pendiente, la tara y un mensaje de peso para mostrar en la pantalla.
- Calcula el peso actual utilizando la información de calibración y la lectura actual de la galga extensiométrica.
- Maneja el parpadeo del LED rojo durante la calibración y la activación de la alarma cuando el sistema está en modo de alarma.
- Calcula el voltaje medio durante las fases de calibración con una serie de muestras.
- Cada función de estado maneja el comportamiento del sistema en un estado específico, actualizando LEDs, la pantalla y realizando acciones según el estado actual.
