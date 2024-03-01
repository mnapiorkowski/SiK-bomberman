# SiK - Bomberman

Computer Networks - 4. semester @ MIMUW

## Description

Bomberman is an online, multiplayer game, in which players can move on the rectangular board and place blocks and bombs. The game consists of three components: server, client and GUI. GUI was provided and is available [here](https://github.com/agluszak/mimuw-sik-2022-public). Communication between client and GUI takes place via UDP. Communication between client and server takes place via TCP. Messeges are serialized using the special protocol. All details are described in `tresc.pdf`. Asynchronous I/O is handled using [Boost.Asio library](https://www.boost.org/doc/libs/1_75_0/doc/html/boost_asio/overview.html).

## How to run

```
make
./robots_{client, server} [parameters]
```
Programs parameters are described in `tresc.pdf`.
