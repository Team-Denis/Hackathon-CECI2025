

from DenisFormat import *


if __name__ == "__main__":
    
    encoder: DenisEncoder = DenisEncoder(1)
    decoder: DenisDecoder = DenisDecoder(1)

    buffer: bytes = open(r'res/test.txt', 'rb').read()
    encoder.Encode(r'res/test.denis', buffer, DenisFormat.TXT)

    buffer = decoder.Decode(r'res/test.denis')
    print(buffer.decode('utf-8'))