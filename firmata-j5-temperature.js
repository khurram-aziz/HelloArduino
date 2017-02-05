var EtherPortClient = require("etherport-client").EtherPortClient;
var firmata = require("firmata");
var five = require("johnny-five");
var request = require('request');

var esp8266 = "192.168.0.111";
var analogPin = 0;
var thingSpeakKey = "";
var temp = 0.0; //global variable for temperature change detection in event

var io = new firmata.Board(new EtherPortClient({ host: esp8266, port: 3030 }));

function round(value, precision) {
    var multiplier = Math.pow(10, precision || 0);
    return Math.round(value * multiplier) / multiplier;
}

io.once('ready', function () {
    console.log('IO Ready');
    io.isReady = true;
    var board = new five.Board({ io: io, repl: false });
    board.on("ready", function () {
        console.log('five ready');
        var temperature = new five.Thermometer({
            //controller: "LM35", //we are using customized toCelsius as esp is powered by 3.3V
            pin: analogPin,
            freq: 1000,
            toCelsius: function(raw) {
                return round(raw * 3300.0 / 1023.0 / 10.0, 1);
            }
        });
        temperature.on("data", function () {
            if (this.C !== temp) {
                temp = this.C;
                console.log("%dC", this.C);
                request.post('https://api.thingspeak.com/update.json', {
                    json: {
                        api_key: thingSpeakKey,
                        field3: temp
                    }, function(error, response, body) {
                        if (error) {
                            console.log(error);
                            console.log(body);
                        }
                    }
                });
            }
        });
    });
});
