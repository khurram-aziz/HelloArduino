var EtherPortClient = require("etherport-client").EtherPortClient;
var firmata = require("firmata");
var j5 = require("johnny-five");
var request = require('request');

var esp8266 = "192.168.0.111";
var ledPin = 0;

var io = new firmata.Board(new EtherPortClient({ host: esp8266, port: 3030 }));

io.once('ready', function () {
    console.log('IO Ready');
    io.isReady = true;
    var board = new j5.Board({ io: io, repl: false });
    board.on("ready", function () {
        console.log('j5 ready');
        var led = new j5.Led(ledPin);
        led.blink();
    });
});
