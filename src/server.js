var port = 3000;
var wssOptions = {
  port: port,
  clientTracking: true,
  // we know we're dealing with quite small messages, let's try to increase security
  maxPayload: 30
};
var WebSocketServer = require('ws').Server
    , wss = new WebSocketServer(wssOptions);
const uuid = require('uuid/v4');

// Broadcast to all.
wss.broadcast = function broadcast(data) {
  wss.clients.forEach(function each(client) {
    client.send(data);
  });
};

var minutes = 5;

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
          var message = 'trigger_on|' + minutes;
          console.log('Sending to client ' + client.clientId, message);
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
