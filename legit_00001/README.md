# DEFCON Quals CTF

## Author Information
hj

### LegitBS

## Description:

This CB accepts a 4 byte data length and then that many bytes of data.

A frequency graph is then generated based upon the bytes sent.

## Vulnerability

The stack has a statically sized buffer of 512 bytes. If a user sends greater than that then they can gain control of execution.

## Challenges

This is a very basic challenge and should not present much difficulty. The primary purpose is for it to serve as an example for a type 1 vulnerability.
