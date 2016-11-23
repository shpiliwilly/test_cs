#!/usr/bin/env python

import sys
import random
import string

from random import choice
from random import randint
from string import ascii_uppercase
from sets import Set

#########################################
def GenerateStrings(count, length):
    strings = []
    for i in range(count):
        strings.append(''.join(random.choice(string.ascii_uppercase + string.digits) for _ in range(length)))

    s = set(strings)
    return list(s)

#########################################
def GenerateRandomOrder(id, traders, stocks):
    order_str = 'id:' + str(id)
    order_str += '|trader:' + traders[randint(0, len(traders) - 1)]
    order_str += '|stock:' + stocks[randint(0, len(stocks) - 1)]
    order_str += '|quantity:' + str(randint(1, 10000))
    order_str += '|side:' + str(randint(0, 1))+ '|'
    return order_str


#########################################
def main():

    trader_count = 1000
    stocks_count = 100
    orders_count = 1000000

    argc = len(sys.argv)
    if argc == 2:
        orders_count = int(sys.argv[1])
    elif argc > 2:
        print 'invalid arguments'
        return

    traders = GenerateStrings(trader_count, 9)
    stocks = GenerateStrings(stocks_count, 6)

    for i in range(orders_count):
        print GenerateRandomOrder(i, traders, stocks)


#########################################

if __name__ == '__main__':
    main()
