MapData = open("crunchtime.map","r").read().split("\n")
Misdirection = open("crunchtime","rb").read()
dataline = " .text.addstring_unlikely"
for Line in xrange(0, len(MapData)):
	if MapData[Line][0:len(dataline)] == dataline:
		AddString = MapData[Line+1].split()
		StartPos = int(AddString[0][2:], 16) & 0xfffff
		EndPos = StartPos + int(AddString[1][2:], 16)
		NewMisdirection = Misdirection[0:StartPos]
		NewMisdirection += chr(0x9c)
		for i in xrange(StartPos, EndPos):
			if Misdirection[i] == chr(0x9c):
				NewMisdirection += Misdirection[StartPos:i]
				break

		for x in xrange(EndPos, i, -1):
			if Misdirection[x] == chr(0x9d):
				NewMisdirection += Misdirection[i+1:x]
				NewMisdirection += Misdirection[x+1:EndPos-1]
				break

		NewMisdirection += chr(0x9d)
		NewMisdirection += Misdirection[EndPos-1:]
		break

open("crunchtime","wb").write(NewMisdirection)
