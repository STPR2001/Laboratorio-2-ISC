const express = require("express");
const http = require("http");
const { Server } = require("socket.io");
const { SerialPort, ReadlineParser } = require("serialport");

const app = express();
const server = http.createServer(app);
const io = new Server(server);

const arduinoPort = new SerialPort({
  path: "/home/lucas/Escritorio/isc/virtual1",
  baudRate: 9600,
});

const parser = arduinoPort.pipe(new ReadlineParser({ delimiter: "\r\n" }));

app.use(express.static("public"));

io.on("connection", (socket) => {
  console.log("Cliente conectado");

  socket.on("motor-command", (command) => {
    console.log("Comando recibido:", command);
    arduinoPort.write(JSON.stringify(command) + "\n");
  });

  parser.on("data", (data) => {
    console.log("Estado del Arduino:", data);
    socket.emit("motor-status", data);
  });

  socket.on("disconnect", () => {
    console.log("Cliente desconectado");
  });
});

const PORT = 3000;
server.listen(PORT, () => {
  console.log(`Servidor corriendo en http://localhost:${PORT}`);
});
