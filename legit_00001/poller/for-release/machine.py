from generator.actions import Actions
import string
import random
import struct

class LEGIT_00001(Actions):

	def start( self ):
		self.read(delim=': ', expect='Enter the length: ')

		l = random.randint( 10,50)

		a = 'a'*l

		self.write( struct.pack('I', len(a)))
		self.write( a )

		self.read(delim=': ', expect='Freq Anal Incoming: ')
		self.read(length=256)

	def end(self):
		return
