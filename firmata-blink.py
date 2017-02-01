import time
from pyfirmata import Arduino, util

board = Arduino('COM3')
led = 13

while True:
    try:
        board.digital[led].write(1)
        time.sleep(1)
        board.digital[led].write(0)
        time.sleep(1)
    except KeyboardInterrupt:
        board.digital[led].write(0)
        break
