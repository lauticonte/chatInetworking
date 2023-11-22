# CPCS 324 - CustomProtocolChatSystem

## DESCRIPCIÓN

Este proyecto es una aplicación de chat cliente-servidor que permite a múltiples usuarios comunicarse entre sí a través de un servidor central. Proporciona una solución interactiva para enviar mensajes públicos y ver la conversación en tiempo real.

La aplicación consta de dos partes: el servidor y el cliente, los cuales se comunican mediante un protocolo personalizado. El servidor, implementado en C, administra las conexiones de los clientes, almacena los mensajes públicos y distribuye estos mensajes a todos los clientes conectados. El cliente, también desarrollado en C, ofrece una interfaz de línea de comandos para interactuar con el servidor, permitiendo enviar mensajes públicos y visualizar la conversación.


## REQUISITOS

- [GCC](https://gcc.gnu.org/)
- [Wireshark](https://www.wireshark.org/)


## COMPILAR Y EJECUTAR (SERVIDOR)

1. Abre una terminal.
2. Navega al directorio que contiene el archivo del servidor (`server.c`).
3. Compila el código del servidor usando el siguiente comando:
    ```bash
    gcc -o server servidor.c -pthread
    ```
4. Ejecuta el servidor:
    ```bash
    ./server
    ```

## COMPILAR Y EJECUTAR (CLIENTE)

1. Abre otra terminal, la necesita para ejecutar el cliente.
2. Navega al directorio que contiene el archivo del cliente (`cliente.c`).
3. Compila el código del cliente usando el siguiente comando:
    ```bash
    gcc -o client cliente.c 
    ```
4. Ejecuta el cliente:
    ```bash
    ./client
    ```

## PROTOCOLO

El protocolo personalizado consta de dos comandos:

- **PUBLIC [message]**: Permite enviar un mensaje público al servidor para que lo distribuya a todos los clientes.
- **VIEW**: Solicita al servidor que envíe todos los mensajes públicos almacenados.

### Descripción del Protocolo

El protocolo personalizado se basa en el protocolo HTTP, por lo que los mensajes enviados por el cliente y el servidor tienen el siguiente formato:

```
[command] [message]
```

Donde:
- **command**: Es el comando del protocolo personalizado.
- **message**: Es el mensaje enviado por el cliente o el servidor.


### Comando PUBLIC

El comando `PUBLIC` permite enviar un mensaje público al servidor para que lo distribuya a todos los clientes.

**Ejemplo:**

```bash
PUBLIC Hola Mundo!
```

### Comando VIEW

El comando `VIEW` solicita al servidor que envíe todos los mensajes públicos almacenados.

**Ejemplo:**

```bash
VIEW
```

## WIRESHARK

Wireshark es una herramienta que permite analizar el tráfico de red. Para monitorear la comunicación entre el cliente y el servidor:

1. Abre Wireshark.
2. Selecciona la interfaz de red por la que se está comunicando el servidor y el cliente.
3. Aplica un filtro para mostrar únicamente el tráfico relacionado con la dirección IP del servidor (`127.0.0.1` o la dirección IP donde se aloja el servidor).
4. Filtro: 
```
tcp.port == 8080 || udp.port == 8080
```
5. Inicia la captura de paquetes.

Esto permitirá ver los paquetes de red enviados y recibidos entre el cliente y el servidor, mostrando los mensajes y comandos del protocolo personalizado.

---

## CONTRIBUCION

-  [Bonasegla, Ezequiel](https://github.com/ezebonasegla)
-  [Conte, Lautaro Manuel](https://github.com/lauticonte)
-  [Ramirez, Gonzalo](https://github.com/gonzaloramireziti)

## LICENCIA

[UTN-FRLP](https://www.frlp.utn.edu.ar/) - Cátedra de Internetworking - 2023



