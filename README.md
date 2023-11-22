# CPCS 324 - CustomProtocolChatSystem

## Compilar y Ejecutar el Servidor

1. Abre una terminal.
2. Navega al directorio que contiene el archivo del servidor (`server.c`).
3. Compila el código del servidor usando el siguiente comando:
    ```bash
    gcc server.c -o server -pthread
    ```
4. Ejecuta el servidor:
    ```bash
    ./server
    ```

## Compilar y Ejecutar el Cliente

1. Abre otra terminal.
2. Navega al directorio que contiene el archivo del cliente (`client.c`).
3. Compila el código del cliente usando el siguiente comando:
    ```bash
    gcc client.c -o client
    ```
4. Ejecuta el cliente:
    ```bash
    ./client
    ```

## Protocolo Personalizado

El protocolo personalizado consta de dos comandos:

- **PUBLIC [message]**: Permite enviar un mensaje público al servidor para que lo distribuya a todos los clientes.
- **VIEW**: Solicita al servidor que envíe todos los mensajes públicos almacenados.

## Probar con Wireshark

Wireshark es una herramienta que permite analizar el tráfico de red. Para monitorear la comunicación entre el cliente y el servidor:

1. Abre Wireshark.
2. Selecciona la interfaz de red por la que se está comunicando el servidor y el cliente.
3. Aplica un filtro para mostrar únicamente el tráfico relacionado con la dirección IP del servidor (`127.0.0.1` o la dirección IP donde se aloja el servidor).
4. Filtro: 
```
tcp.port == 8080 || udp.port == 8080
```
4. Inicia la captura de paquetes.

Esto permitirá ver los paquetes de red enviados y recibidos entre el cliente y el servidor, mostrando los mensajes y comandos del protocolo personalizado.

---

