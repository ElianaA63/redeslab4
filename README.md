# Informe Laboratorio 4 - Capa de Red

----

## Tarea de Analisis 1)

### Uso de buffers

![graficabuffer1](./GraficasAnalisis/Buffer.png)

En esta grafica se puede ver que el nodo 0, es el que mas se carga, dado que encola todos los paquetes que genera el mismo, mas los paquetes que genera el nodo 2, porque como siempre se envian los paquete a la derecha no se usan los demas nodos.

![graficabuffer2](./GraficasAnalisis/Bufferwo0.png)

Aca podemos ver que el nodo 2 es el siguiente mas exigido porque es el otro nodo generador.

![graficabuffer3](./GraficasAnalisis/Bufferwo02.png)

En esta otra grafica se observa que los nodos 3 y 4 no estan siendo usados en la red, ya que el nodo destino es el 5 y siempre enviamos los paquetes hacia la derecha.
Los nodos 1, 7 y 6 tiene una carga variable entre 0 y 1 debido que solo reenvian los datos generados por los nodos 0 y 2 y ellos no generan paquetes.
El nodo 5 no usa su buffer porque es el receptor y los envia directamente a la capa de Aplicacion.

Dado que que algoritmo de enrutamiento es infeciente tenemos 2 nodos inutilizados que podrian mejorar el uso de buffers.
Si el interArrivalTime fuera menor, el noso 0 se congestionaria y podria llegar a perder paquetes.

### Delay

![graficadelay](./GraficasAnalisis/DelayPerPacket.png)

El tiempo de envio de los paquetes aumenta durante el tiempo se simulacion, por la sobresaturacion de algunos nodos y la inutilizacion de otros.

### Hops

![graficahops](./GraficasAnalisis/HopsPerPacket.png)

Los paquetes pasan por 4-6 paquetes, los que se generan en el nodo 0 usan 4 nodos para llegar al destino (camino minimo en distancia) y los que se generan en el nodo 2 usan 6 nodos para llegar al nodo 5, que no es el camino mas eficiente, ni en distancia ni tiempo.

## Tarea de Analisis 2)

A continucion veremos distintas graficas modificando el interArrivalTime de los nodos tratando de lograr una estabilizacion en los buffers.

**interArrivalTime = 1**
![graficabuffer1](./GraficasAnalisis/Buffers/Buffer1.png)

**interArrivalTime = 1.5**
![graficabuffer1.5](./GraficasAnalisis/Buffers/Buffer1_5.png)

**interArrivalTime = 2**
![graficabuffer2](./GraficasAnalisis/Buffers/Buffer2_0.png)

**interArrivalTime = 3.5**
![graficabuffer3.5](./GraficasAnalisis/Buffers/Buffer3_5.png)

**interArrivalTime = 6.0**
![graficabuffer6](./GraficasAnalisis/Buffers/Buffer6_0.png)

En el ultimo grafico consideramos que ya estan equilibrados ya que solo hay una diferencia maxima de 20 paquetes.
Como el ```interArrivalTime``` es de 6 la generacion de paquetes es muy poca y lenta.

----

## Tarea de Diseño

Todas las graficas y comparativas son con un ```interArrivalTime``` = 1

### Routing Table

Nuestra primera decision fue hacer en cada modulo Net una routing table que contiene el camino que debe seguir el paquete, segun sea su destino.
Cada posicion de la tabla contiene la mejor decision (izquierda o derecha) para cada nodo.

### Comparacion de buffers

| Antes 										  | Después 											   |
| :---------------------------------------------: | :----------------------------------------------------: |
| ![](./GraficasAnalisis/Buffer.png)              | ![](./GraficasDiseno/D1buffers.png)                    |

Como se puede ver los buffer se estabilizaron, el 1 y 2 no se usan casi, porque son los unicos que generan y no les llega nada

### Comparacion de delay

| Antes 										  | Después 											   |
| :---------------------------------------------: | :----------------------------------------------------: |
| ![](./GraficasAnalisis/Delay1_0.png)            | ![](./GraficasDiseno/D1delay.png)                      |

Gracias al algoritmo, en el mismo tiempo logramos enviar el doble de paquetes, y con menor delay.

### Comparacion de hops

| Antes 										  | Después 											   |
| :---------------------------------------------: | :----------------------------------------------------: |
| ![](./GraficasAnalisis/Hops1_0.png)             | ![](./GraficasDiseno/D1hops.png)                       |

Sin nuestro algoritmo la cantidad de saltos de cada paquete llega hasta 8 (contando el nodo destino y emisor), y despues de la implementacion no supera los 5 saltos.

Sabemos que la idea de tener una tabla hardcodeada no es la solucion mas elegante, pero al ser una red bastante chica y con topologia circular era lo mas eficiente.
Una idea que tuvimos para hacer la tabla de forma dinamica es que cada nodo envie un paquete "Hello" tanto por izquierda como por la derecha y, aprovechando la topologia circular, que de toda la vuelta contando cuantos saltos tiene a cada nodo, y cuando regresen los dos paquetes comparar los resultados y asi llenar la tabla con la mejor decision.

----

## Tarea Estrella

Usamos el algoritmo de inundacion.

Idea general: para enviar un paquete de un origen "u" a un destino "v" los caminos usados son aquellos que respetan las siguientes reglas:

- "u" manda el mensaje por todas las lineas de salida
- cada paquete llega a un enrutador distinto de "v", y se reenvia por cada unas de las lineas excepto por la que llego

El problema de este algoritmo es que genera grandes cantidades de paquetes duplicados congestionando la red; a menos que se tomen algunas medidas para limitar el proceso.

1) **Inundacion con numero de secuencia**:
 A cada paquete se le asigna un numero de secuencia, asi cada nodo diferente a "v" recuerda si lo vio y lo reenvio, y si llega algun duplicado lo elimina.
Para esto cada nodo tiene una tabla de paquetes difundidos donde guarda una lista de numeros de secuancia vistos para cada nodo.
Para actualizar la tabla tiene que llegar un paquete con un numero de secuencia no visto o un paquete de un nodo origen que nunca mando nada.

2) **Inundacion con contador de saltos**:
 Integramos un contador de saltos en el paquete, que disminuye a cada salto, y cuando llega a cero el paquete es eliminado, excepto que haya llegado al nodo destino.
El contador de saltos se determina con la distancia entre el nodo origen y el destino, en caso de desconocerla se puede iniciar con el diametro (distancia maxima) de la red.

Para saber la distancia entre los nodos implementamos un algoritmo con la siguiente idea.
Cada nodo manda un paquete ```hello```, por inundacion, con un contador de saltos que incrementa por cada nodo que pasa, cuando un nodo lo recibe guarda el numero de saltos que tiene el paquete (distancia entre el nodo que lo envio y el que lo recibio), aumenta en 1 el contador y lo reenvia, antes de eso hace una verificacion de no haber visto otro paquete con un contador de saltos menor, de ser asi lo descarta.
Al final, todos los nodos conocen la distancia a cada uno de los otros nodos, y con esa informacion al crear un paquete inician el contador de saltos.

Con estas dos mejoras evitamos que se generen muchos paquetes duplicados y que un paquete viaje mucho por la red sin necesidad, y asi se evitan congestiones en las lineas.

Otra idea para optimizar aun mas el codigo es:

3) **Inundacion selectiva**:

. Los nodos no envian los paquetes por todas las lineas, sino por aquellas que van aproximadamente en la direccion correcta.
. Para eso se necesita saber en que direccion va cada linea y en que direccion esta el destino.

Esta idea no pudimos implementarla por cuestiones de tiempo.

![](./TareaEstrella/inundacion.mp4)

----

Intregantes:

1) Arias, Eliana

2) Toyos, Milagros

3) Hak, Gabriel



