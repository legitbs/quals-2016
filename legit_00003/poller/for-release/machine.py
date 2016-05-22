from generator.actions import Actions
import string
import random
import struct

class LEGIT_00003(Actions):

	def start( self ):
		self.read(delim='\n', expect='1) Gimme Name\n')
		self.read(delim='\n', expect='2) Print Name\n')
		self.read(delim='\n', expect='3) Exit\n')
		self.read(delim=': ', expect=': ')

		l = random.randint( 10,20)

		a = ''

		for _ in range(l):
			a += random.choice(string.lowercase)

		self.write( '1\n')
		self.read(delim=': ', expect='Enter Name: ')
		self.write( a + '\n' )

		self.read(delim='\n', expect='1) Gimme Name\n')
		self.read(delim='\n', expect='2) Print Name\n')
		self.read(delim='\n', expect='3) Exit\n')
		self.read(delim=': ', expect=': ')

		self.write('2\n')
		self.read(delim='\n', expect='%s\n' %(a))

		self.read(delim='\n', expect='1) Gimme Name\n')
		self.read(delim='\n', expect='2) Print Name\n')
		self.read(delim='\n', expect='3) Exit\n')
		self.read(delim=': ', expect=': ')

		self.write('3\n')
		self.read(delim='\n', expect='Exit\n')

	def end(self):
		return
