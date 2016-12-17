const port = process.env.PORT || 3000;
const secretToken = process.env.SECRET_TOKEN || 'secret';
const secretPath = '/' + secretToken;
const wssOptions = {
  port: port,
  clientTracking: true,
  // Use the path as "authentication", only allow clients to connect this path
  path: secretPath,
  // we know we're dealing with quite small messages, let's increase security by not allowing large payloads
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
  const httpRequest = ws.upgradeReq;
  // X-Forwarded-For for Heroku
  const ip = httpRequest.headers['x-forwarded-for'] || httpRequest.connection.remoteAddress;

  const idAndIp = id + ' [' + ip + ']';

  ws.clientId = idAndIp;
  console.log('Connect from client', idAndIp);

  ws.on('message', function message(data) {
    console.log('Message from client ' + idAndIp + ', data:', data);
    // Broadcast to everyone else.
    if (data == 'turn_on') {
      wss.clients.forEach(function each(client) {
        if (client !== ws) {
          console.log('Sending to client ' + client.clientId, triggerOnMessage);
          client.send(triggerOnMessage);
        }
      });
    }
  });

  ws.on('close', function (code, reason) {
    console.log('Disconnect from client', id, reason);
  })
});

function pingClients(callback) {
  wss.clients.forEach(function each(client) {
    client.ping('ping');
  });
  callback();
}

function scheduleClientPing() {
  setTimeout(pingClients, 45000, scheduleClientPing);
}

wss.on('listening', function () {
  console.log('Listening on *:' + port);
  scheduleClientPing();
});
