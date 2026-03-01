# Redis From Scratch

## Network Perspective

### Layers

Ethernet -> IP -> UDP/TCP -> Application Protocols

1. IP:
  Hardware only stores small packets (IP packets).    The ability to assemble packets are provided by a higher layer.

2. Port Number: 
  Since the computer has multiple connected to the network so it does "demultiplexing". The next layer of IP (UDP/TCP) adds a 16 bit port number identifing different processes.

  The computer uses 4 tuple to identify a "flow":- 
    1. src_ip
    2. src_port
    3. dst_ip
    4. dst_port

### Sockets

A socket is ***Handle*** to refer to a connection.
Handle is an opaque integer used to refer to things that cross an API boundary.

**Creating a Listening Socket**

1. Obtain a socket handle via `socket()`
2. Set the listening IP:port via `bind()`
3. Create the listening socket via `listen()`

**Connect a Client**

1. Obtain a socket handle via `socket()`
2. Create the connection socket via `connect()`


