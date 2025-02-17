""" Quick script to turn on/off devices using RPI GPIOs """

import argparse
import time
import pigpio

PINS = {'left': 18, 'right': 23}

PI = pigpio.pi('pi')

def toggle(pin, cmd):
    """ Execute the available commands """
    if cmd == 'on':
        PI.write(pin, 1)
    elif cmd == 'off':
        PI.write(pin, 0)
    elif cmd == 'reset':
        PI.write(pin, 0)
        time.sleep(0.5)
        PI.write(pin, 1)

def main():
    """ Our main """
    parser = argparse.ArgumentParser()
    parser.add_argument('pqm', choices=PINS.keys())
    parser.add_argument('cmd', choices=['on', 'off', 'reset'])
    args = parser.parse_args()
    toggle(PINS[args.pqm], args.cmd)
if __name__ == '__main__':
    main()
