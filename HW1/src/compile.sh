#!/bin/sh

g++ Client.cpp Socket.cpp -o client
g++ Server.cpp Socket.cpp -o server
