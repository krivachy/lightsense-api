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

The arduino's use polling, there are 2 endpoints:

```
GET /secret => gets a 1 or 0 depending on if the light should be on
GET /secret/on => turns the light on for TRIGGER_ON_MINUTES minutes
```

To change the word `secret` in the URL slug override `SECRET_TOKEN`

