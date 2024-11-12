const socket = io();

function sendCommand(type, command) {
  socket.emit("motor-command", { type, ...command });
}

function getSteps() {
  return parseInt(document.getElementById("stepsInput").value) || 0;
}

socket.on("motor-status", (data) => {
  const { type, status } = JSON.parse(data);
  if (type === "DC") {
    let partes = status.split("|");
    document.getElementById("dcSentido").innerText = `Sentido: ${partes[0]}`;
    document.getElementById("dcStatus").innerText = `Estado: ${partes[1]}`;
  } else if (type === "PaP") {
    document.getElementById("papStatus").innerText = `Estado: ${status}`;
  } else if (type === "Servo") {
    document.getElementById("servoStatus").innerText = `Estado: Ángulo ${status}`;
  }
});
