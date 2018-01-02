// Config
const port = process.env.PORT || 3000;
const minutes = parseFloat(process.env.TRIGGER_ON_MINUTES || 0.5);
const secretToken = process.env.SECRET_TOKEN || 'secret';
const secretPath = '/' + secretToken;

const Hapi = require('hapi');

const server = new Hapi.Server({ port, host: '0.0.0.0' });

let state = 0;
let timer;

const turnOn = () => {
    console.log('Light turned on');
    state = 1;
};
const turnOff = () => {
    console.log('Light turned off');
    state = 0;
};

function startTimerFor(func) {
    if (timer) {
        clearTimeout(timer);
    }
    const timeout = minutes * 60 * 1000;
    timer = setTimeout(func, timeout);
}

server.route({
    method: 'GET',
    path: secretPath,
    handler: function (request, h) {
        console.log(`[${request.info.referrer || request.info.remoteAddress}] State requested: ${state}`);
        return state;
    }
});

server.route({
    method: 'GET',
    path: secretPath + '/on',
    handler: function (request, h) {
        console.log(`[${request.headers['x-forwarded-for'] || request.info.remoteAddress}] Turn on requested`);
        turnOn();
        startTimerFor(turnOff);
        return state;
    }
});

async function start() {
    try {
        await server.start();
        console.log('Server started');
        console.log(`Config variables: minutes=${minutes} secretToken=${secretToken} port=${port}`)
    } catch (err) {
        console.log(err);
    }
}

start();
