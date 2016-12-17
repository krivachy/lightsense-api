# Lightsense

[![Empowered by Futurice's open source sponsorship program](https://img.shields.io/badge/sponsor-chilicorn-ff69b4.svg)](https://spiceprogram.org?utm_source=github&utm_medium=spice&utm_campaign=krivachy/lightsense-api)

Lightsense is a small concept where you install multiple lights that connect to an arduino and when one is turned on
the others are turned on as well for a short time, therefore providing some "presence" even if the other light(s) aren't
near. This is the bacekend for it.

## Getting started

Perquisite: install [node and npm](https://nodejs.org/en/download/)

Install dependencies:

```
npm install
```

To run:

```
npm start
```

You should see the following output:

```
lightsense-api$ npm start

> lightsense-api@1.0.0 start lightsense-api
> node src/server.js

Listening on *:3000
```

## Trying it out

Connect to the server at `ws://localhost:3000/secret` with 2 WebSocket clients or with this [Chrome extension](https://chrome.google.com/webstore/detail/smart-websocket-client/omalebghpgejjiaoknljcfmglgbpocdp)
in multiple browser tabs.

Send it a simple message as a plain string with one client:

```
turn_on
```

In the other client see the response:

```
trigger_on|5
```

Where the 5 indictes for how many minutes it should turn on.

## Security

In production do the following:
1) Use `wss://` instead of `ws://`
2) Set the SECRET_TOKEN envvar to something actually secret

Since this is meant for arduino devices other authorization or authentication methods are not planned.
