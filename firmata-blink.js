var Board = require("firmata").Board;
var board = new Board("COM3");

var led = 13;
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
    }.bind(this), 500);
});
