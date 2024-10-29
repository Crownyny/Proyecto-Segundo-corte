Sistemas Operativos - Proyecto de segundo corte

Nombres: Julián Meneses, Andrea Realpe.

1. Sistema de Control de Versiones Remoto
Los Sistemas de Control de Versiones (VCS) permiten guardar el rastro de las
modificaciones sobre determinados elementos. En el contexto de este proyecto,
se gestionan versiones de archivos y directorios. Se implementa un sistema de
control de versiones remoto, que permite:
Adicionar un archivo al repositorio de versiones.
Listar las versiones de un archivo en el repositorio de versiones.
Listar todos los archivos almacenados en el repositorio Obtener la versión de un
archivo del repositorio de versiones.
Se deberán crear dos (programas), un programaservidor llamado rversionsd, y un programa cliente llamado rversions.

1.1. Uso del cliente rversions

$ ./rversions 
Uso: rversions IP PORT Conecta el cliente a un servidor en la IP y puerto especificados. Los comandos, una vez que el cliente se ha conectado al servidor,
son los siguientes: add archivo "Comentario" list archivo list get numver archivo

1.2. Uso del servidor rversionsd

$ ./rversionsd 
Uso: rversionsd PORT Escucha por conexiones del cliente en el puerto especificado.

1.3. Repositorio de versiones

El repositorio de versiones funciona como un servidor que mediante sockets permite 
la conexión de uno o más clientes. Una vez iniciado, debe crear un directorio llamado
".versions", el el cual se almacenarán todos los archivos del cliente. Para esta versión
el manejo de directorios y subdirectorios es opcional. El sevidor puede almacenar 
los archivos del cliente en el directorio sin crear subdirectorios.

1.4. Detección de cambios

El esquema para detección de cambios en un archivo puede ser muy complejo. 
Para la implementación de este programa, se usará el módulo auxiliar sha256, el cual 
implementa el algoritmo del mismo nombre y permite obtener el código SHA de 256 bits 
(64 bytes + NULL).
