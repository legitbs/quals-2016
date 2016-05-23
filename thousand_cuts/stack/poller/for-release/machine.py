#!/usr/bin/env python

from generator.actions import Actions
import random


class TemplateGenerator(Actions):
    def start(self):
        1 + 1 #noop

    def echo(self):
        self.write("vito\n")
        self.read(length=4, expect="vito")
