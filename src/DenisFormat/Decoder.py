

from .FormatHelper import EncoderHelper, DenisConstants


class DenisDecoder:

    def __init__(self, version: int) -> None:
        self._version: int = version

    @property
    def version(self) -> int:
        return self._version

    @staticmethod
    def _read_version1_header(buffer: bytes) -> dict:

        assert len(buffer) == DenisConstants.HEADER_LENGTH, f"Buffer length is {len(buffer)} instead of {DenisConstants.HEADER_LENGTH}"

        magic_bytes: str = buffer[0:5]
        version_bytes: bytes = buffer[5:6]
        denis_format_bytes: bytes = buffer[6:9]
        extra_bytes: bytes = buffer[9:16]
        size_bytes: bytes = buffer[16:24]

        magic: str = EncoderHelper._bytes_to_str(magic_bytes)
        assert magic == DenisConstants.DENIS_MAGIC_STRING, f"Invalid magic string: {magic}. Expected: {DenisConstants.DENIS_MAGIC_STRING}."
        version: int = EncoderHelper._bytes_to_int(version_bytes, 1)
        denis_format: str = EncoderHelper._bytes_to_str(denis_format_bytes)
        size: int = EncoderHelper._bytes_to_int(size_bytes, 8)

        return {
            'magic': magic,
            'version': version,
            'denis_format': denis_format,
            'extra': extra_bytes,
            'size': size
        }

    @staticmethod
    def _read_header(buffer: bytes, version: int) -> dict:
        
        match version:
            case 1:
                return DenisDecoder._read_version1_header(buffer)
            case _:
                raise ValueError(f"Invalid version: {version}.")
        
    @staticmethod
    def _read_file(buffer: bytes, version: int) -> bytes:

        header: dict = DenisDecoder._read_header(buffer[0:DenisConstants.HEADER_LENGTH], version)
        content: bytes = buffer[DenisConstants.HEADER_LENGTH:DenisConstants.HEADER_LENGTH + header['size']]
        terminator: bytes = buffer[DenisConstants.HEADER_LENGTH + header['size']:DenisConstants.HEADER_LENGTH + header['size'] + len(DenisConstants.TERMINATOR)]

        assert terminator == DenisConstants.TERMINATOR, f"Invalid terminator: {terminator}. Expected: {DenisConstants.TERMINATOR}."

        return content
        
    def Decode(self, fp: str) -> bytes:

        buffer = EncoderHelper._read_buffer(fp)
        assert buffer, f"Buffer is empty."

        return self._read_file(buffer, self.version)
    