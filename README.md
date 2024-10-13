# TCP-Video-Sending
 
This project implements a simple file transfer system using TCP sockets in C. It consists of two programs: a server (receiver) and a client (sender). The client sends a file to the server, and the server saves the file with a modified name.

 Prerequisites
- C compiler (e.g., GCC)
- Linux/Unix-based operating system (for socket programming)

Files
receiver.c - This is the receiver program that listens for incoming connections and receives the file sent by the client.
sender.c - This is the sender program that reads a file from the local system and sends it to the server.

 How to Compile
To compile the server and client programs, use the following commands:
gcc -o sender sender.c
gcc -o receiver receiver.c

 How to Run

 Step 1: Start the Server
Run the receiver program on your machine. The receiver listens for incoming file transfers on port 8000.
./receuver
Once the server starts, it will listen for connections from the client:
Listening for incoming connections...

Step 2: Run the Client
Run the sender program and provide the file path of the file you want to send as an argument:

./sender <file_to_send>

 What Happens:
- The client establishes a connection to the server (running on `127.0.0.1` on port `8000`).
- The client sends the filename to the server.
- The client reads the file in chunks and sends it over the connection.
- The server receives the file, appends "_received" to the original filename, and saves the file.
- Once the file transfer is complete, the server sends a success message back to the client.

 Example Output:

**Server:**

```
Listening for incoming connections...
[ + ] Client(Sender) connected
[ + ] Receiving file and saving as: myfile_received.txt
[ + ] File received successfully
[ + ] Connection closed. Ready to accept new connections.
Listening...
```

**Client:**

```
[ + ] Connected to server
[ + ] Sending file: myfile.txt
[ + ] File sent successfully
[ + ] Server(Receiver) response: File received successfully
[ + ] Connection closed
```

 Functionality Overview

- **Server Side**: Listens for incoming connections, receives the file, and saves it with the "_received" suffix.
- **Client Side**: Connects to the server, sends the file in chunks, and waits for an acknowledgment.