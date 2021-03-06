var Board = require("firmata").Board;
var EtherPortClient = require("etherport-client").EtherPortClient;
var request = require('request');

var esp8266 = "192.168.0.111";
var led = 0;
var analog = 0;
var thingSpeakKey = "";
var temp = 0.0; //global variable for temperature change detection in event

var board = new Board(
        new EtherPortClient({ host: esp8266, port: 3030 })); 

function round(value, precision) {
    var multiplier = Math.pow(10, precision || 0);
    return Math.round(value * multiplier) / multiplier;
}

board.on("ready", function() {
    console.log(this.firmware);
    this.pinMode(analog, this.MODES.OUTPUT);
    this.pinMode(led, this.MODES.OUTPUT);

    process.on('SIGINT', function() {
        console.log("Turning led off");
        board.digitalWrite(led, 0);
        process.exit();
    });

    var state = 1;
    setInterval(function () {
        this.digitalWrite(led, (state ^= 1));
        board.analogRead(analog, function(analogValue) {
            //LM35
            var mv = analogValue * 3300.0 / 1023.0;    //esp is powered by 3.3V
            var tempC = mv / 10.0;
            var tempF = (tempC * 1.8) + 32;
            var cValue = round(tempC, 1);
            var fValue = round(tempF, 1);
            if (temp !== cValue) { //only interested if temperature is changed
                temp = cValue;
                console.log("Reading: " + analogValue + ", mv: " + mv + ", tempC: " + tempC + ", " + cValue + "C");
                request.post('https://api.thingspeak.com/update.json', {
                    json: {
                        api_key: thingSpeakKey,
                        field1: analogValue, field2: mv, //good idea to send/store raw data
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
    }.bind(this), 5000); //each 5s
});
