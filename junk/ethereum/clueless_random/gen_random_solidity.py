' Generate random solidity program '

import random

NREG = 5

def get_program(random_body):
    ' Get the random body to use. '

    registers = ''
    for idx in range(NREG):
        registers += 'uint r{} = 1;'.format(idx)

    return '''contract SimpleStorage { REGISTERS uint[2**20] store; function iterate() { RANDOM_BODY }}'''.replace('RANDOM_BODY', random_body).replace('REGISTERS', registers)

def get_uint_var():
    ' Get a variable uint.'
    available = [
        'block.difficulty',
        'block.gaslimit',
        'block.number',
        'block.timestamp',
        'msg.gas',
        'msg.value',
        'tx.gasprice',
    ]
    return random.choice(available)

def get_uint_slot():
    ' Get one of the storage slot. '
    assert NREG > 0
    return 'r{}'.format(random.randint(0, NREG - 1))

def get_uint_op():
    ' Get an operator '
    return random.choice(['+', '*'])

def gen_expression(max_depth):
    ' Get a random expression. '
    if max_depth == 0:
        return random.choice([get_uint_slot(), get_uint_var()])
    return '({} {} {})'.format(gen_expression(max_depth - 1),
                               get_uint_op(),
                               gen_expression(max_depth - 1))

def gen_iteration():
    ' Generate the iteration. '
    exp = ''
    for idx in range(NREG):
        reg_name = 'r{}'.format(idx % NREG)
        exp += '{} = {};'.format(reg_name, gen_expression(3))
        exp += 'store[({} + 1) % (2 ** 20)] = {};'.format(reg_name, reg_name)
    return exp

print(get_program(gen_iteration()))
