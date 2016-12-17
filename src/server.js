const port = process.env.PORT || 3000;
const wssOptions = {
  port: port,
  clientTracking: true,
  // we know we're dealing with quite small messages, let's try to increase security
  maxPayload: 30
};
const WebSocketServer = require('ws').Server
    , wss = new WebSocketServer(wssOptions);
const uuid = require('uuid/v4');

// Broadcast to all.
wss.broadcast = function broadcast(data) {
  wss.clients.forEach(function each(client) {
    client.send(data);
  });
};

const minutes = 5;
const triggerOnMessage = 'trigger_on|' + minutes;

wss.on('connection', function connection(ws) {
  const id = uuid();
  ws.clientId = id;
  console.log('Connect from client', id);
  ws.on('message', function message(data) {
    console.log('Message from client ' + id + ', data:', data);
    // Broadcast to everyone else.
    if (data == 'turn_on') {
      wss.clients.forEach(function each(client) {
        if (client !== ws) {
          console.log('Sending to client ' + client.clientId, triggerOnMessage);
          client.send(message);
        }
      });
    }
  });
  ws.on('close', function (code, reason) {
    console.log('Disconnect from client', id, reason);
  })
});

wss.on('listening', function () {
  console.log('Listening on *:' + port);
});
