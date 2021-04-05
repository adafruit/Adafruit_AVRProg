from intelhex import IntelHex
ih = IntelHex()                     # create empty object
ih.loadhex('rc_calib_tiny85.hex')               # load from hex
for i in range(32):
    if ih[i]==255:
        ih[i]=255

ih.dump()

from cStringIO import StringIO
sio = StringIO()
ih.write_hex_file(sio)
hexstr = sio.getvalue()
sio.close()

print("\n\n\n\n")
print(hexstr)

