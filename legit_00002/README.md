# DEFCON Quals CTF

## Author Information
hj

### LegitBS

## Description:

This CB accepts a 4 byte data length and then that many bytes of data.

It then adds up and if the value is 31337 it returns success otherwise failure

## Vulnerability

The stack has a statically sized buffer of 804 bytes. The user will be able to overwrite the pointer that displays the sucess string. This allows the user to leak out data from the magic page.

## Challenges

This is a very basic challenge and should not present much difficulty. The primary purpose is for it to serve as an example for a type 2 vulnerability.
