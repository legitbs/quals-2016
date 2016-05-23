from generator.actions import Actions
import string
import random
import struct

class LEGIT_00002(Actions):

	def start( self ):
		self.read(delim=': ', expect='Enter the length: ')

		l = random.randint( 10,50)
		a = ''

		total = 0

		for _ in range(l):
			b = random.randint(0, 255)
			total += b
			a += chr(b)

		self.write( struct.pack('I', len(a)))
		self.write( a )

		if total == 0x31337:
			self.read( delim='\n', expect='SUCCESS: 201527\n')
		else:
			self.read(delim='\n', expect='FAIL: %d\n' %(total))

	def end(self):
		return
