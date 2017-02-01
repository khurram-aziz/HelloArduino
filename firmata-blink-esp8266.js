var Board = require("firmata").Board;
var EtherPortClient = require("etherport-client").EtherPortClient;

var esp8266 = "192.168.0.91";
var led = 0;

var board = new Board(
    new EtherPortClient({ host: esp8266, port: 3030 }));
board.on("ready", function() {
    console.log(this.firmware);
    this.pinMode(led, this.MODES.OUTPUT);

    process.on('SIGINT', function() {
        console.log("Turning led off");
        board.digitalWrite(led, 0);
        process.exit();
    });

    console.log("Blinking led");
    var state = 1;
    setInterval(function () {
        this.digitalWrite(led, (state ^= 1));
    }.bind(this), 5000);
});
