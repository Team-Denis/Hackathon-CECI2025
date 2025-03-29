

from .FormatHelper import EncoderHelper, DenisFormat, DenisConstants


class DenisEncoder:

    def __init__(self, version: int) -> None:
        self._version: int = version

    @property
    def version(self) -> int:
        return self._version
    
    @staticmethod
    def _get_version1_header(bytes: bytes, denis_format: DenisFormat) -> bytes:
        
        buffer = b''

        buffer += EncoderHelper._str_to_bytes(DenisConstants.DENIS_MAGIC_STRING)
        buffer += EncoderHelper._int_to_byte(1, 1)                 # version
        buffer += EncoderHelper._str_to_bytes(denis_format.value)  # format on 3 bytes
        buffer += b'\x00' * DenisConstants.EXTRA_HEADER_LENGTH     # extra 7 bytes free
        buffer += EncoderHelper._int_to_byte(len(bytes), 8)        # size of file (just the bytes, on 8 bytes)

        # buffer should be 24 bytes long
        assert len(buffer) == DenisConstants.HEADER_LENGTH, f"Buffer length is {len(buffer)} instead of {DenisConstants.HEADER_LENGTH}"

        return buffer

    @staticmethod
    def _get_header(buffer: bytes, version: int, dformat: DenisFormat) -> bytes:
        
        match version:
            case 1:
                return DenisEncoder._get_version1_header(buffer, dformat)
            case _:
                raise ValueError(f"Invalid version: {version}.")

    def Encode(self, fp: str, buffer: bytes, denis_format: DenisFormat) -> None:

        assert denis_format != DenisFormat.NONE, "Format cannot be NONE."
        assert denis_format in DenisFormat, f"Invalid format : {denis_format}."

        buffer_to_write = b''
        buffer_to_write += DenisEncoder._get_header(buffer, self.version, denis_format)
        buffer_to_write += buffer
        buffer_to_write += DenisConstants.TERMINATOR

        # write the buffer to the file
        bytes_written = EncoderHelper._write_buffer(fp, buffer_to_write)
        assert bytes_written == len(buffer_to_write), f"Bytes written {bytes_written} does not match buffer length {len(buffer_to_write)}."

