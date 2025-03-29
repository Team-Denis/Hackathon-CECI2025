

import os
from enum import Enum


class DenisFormat(Enum):

    IMG: str = 'img'
    TXT: str = 'txt'
    NONE = ...


class DenisConstants:

    DENIS_MAGIC_STRING: str = 'DENIS'
    HEADER_LENGTH: int = 24
    EXTRA_HEADER_LENGTH: int = 7
    TERMINATOR: bytes = b'\xff' * 8


class EncoderHelper:

    @staticmethod
    def _write_buffer(fp: str, buffer: bytes) -> int:

        try:
            with open(fp, 'wb') as file:
                return file.write(buffer)
        except IOError as e:
            raise IOError(f"Error writing to file {fp}: {e}")
        
    @staticmethod
    def _read_buffer(fp: str) -> bytes:
        
        assert os.path.exists(fp), f"File {fp} does not exist."
        assert os.path.isfile(fp), f"Path {fp} is not a file."
        
        try:
            with open(fp, 'rb') as file:
                return file.read()
        except IOError as e:
            raise IOError(f"Error reading from file {fp}: {e}")

    @staticmethod
    def _str_to_bytes(s: str) -> bytes:
        return s.encode('utf-8')

    @staticmethod
    def _int_to_byte(i: int, length: int = 1) -> bytes:
        return i.to_bytes(length, byteorder='big')


