"""Generate the four TH04 MAIN sprite owners for a bounded link diagnostic.

The original Tupfile feeds these ignored reference BMPs through ReC98's
``bmp2arr`` tool and includes the resulting ``.asp`` files in MAIN's DATA
segment.  The product tree must not carry the game assets, so this helper
replays only that deterministic source-to-ASM step in a private probe tree.
It is deliberately not an exactness or provenance claim: the input hashes
are pinned to the locally supplied reference files and every generated source
is discarded with the probe worktree.
"""

from __future__ import annotations

from dataclasses import dataclass
import hashlib
from pathlib import Path
import struct


@dataclass(frozen=True)
class SpriteAsset:
    symbol: str
    input_path: str
    sprite_width: int
    sprite_height: int
    preshift: str | None
    expected_sha256: str


SPRITE_ASSETS = (
    SpriteAsset(
        "_sPELLET",
        "_reference/ReC98/th02/sprites/pellet.bmp",
        8,
        8,
        "outer",
        "220a46e267056dc4af64b80942721bc1f4679f090ae1b6d9c88b9bab02b3da70",
    ),
    SpriteAsset(
        "_sPELLET_BOTTOM",
        "_reference/ReC98/th04/sprites/pelletbt.bmp",
        8,
        4,
        "outer",
        "a33ec482eb419620752cbe40c8979e7d6cb2c537018252a091390f7f2fcad646",
    ),
    SpriteAsset(
        "_sPOINTNUMS",
        "_reference/ReC98/th04/sprites/pointnum.bmp",
        8,
        8,
        "inner",
        "e961b7b02ba0bb3f43df060b3cee07f73d3d26fb54e03e8a16d17383342b342d",
    ),
    SpriteAsset(
        "_sSPARKS",
        "_reference/ReC98/th02/sprites/sparks.bmp",
        8,
        8,
        "outer",
        "58d15248a7c1da6a8fe635d5ff73913e2c7935c203330e4460e585682bc7a0b0",
    ),
)


def _sha256(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def _load_bmp_1bpp(root: Path, spec: SpriteAsset) -> tuple[bytes, int, int]:
    path = root / spec.input_path
    data = path.read_bytes()
    digest = _sha256(data)
    if digest != spec.expected_sha256:
        raise ValueError(
            f"sprite input identity drift: {spec.input_path} "
            f"{digest} != {spec.expected_sha256}"
        )
    if data[:2] != b"BM" or len(data) < 54:
        raise ValueError(f"invalid BMP header: {spec.input_path}")

    pixel_offset = struct.unpack_from("<I", data, 10)[0]
    dib_size = struct.unpack_from("<I", data, 14)[0]
    if dib_size < 40:
        raise ValueError(f"unsupported BMP DIB: {spec.input_path}")
    width, height = struct.unpack_from("<ii", data, 18)
    planes, bpp = struct.unpack_from("<HH", data, 26)
    compression = struct.unpack_from("<I", data, 30)[0]
    if planes != 1 or bpp != 1 or compression not in (0, 3):
        raise ValueError(f"sprite is not an uncompressed 1bpp BMP: {spec.input_path}")
    if width < 1 or height < 1:
        raise ValueError(f"top-down/empty BMP is unsupported: {spec.input_path}")
    src_stride = ((width * bpp + 31) & ~31) // 8
    end = pixel_offset + (height * src_stride)
    if end > len(data):
        raise ValueError(f"truncated BMP pixels: {spec.input_path}")

    # bmp2arr accepts palettes with white first and flips the source bits in
    # that case.  The supplied 1bpp inputs use black first, but preserve the
    # original tool's rule rather than silently assuming a palette order.
    palette_offset = 14 + dib_size
    xor_value = 0
    if palette_offset + 3 < len(data):
        if any(data[palette_offset + index] & 0x80 for index in range(3)):
            xor_value = 0xFF

    rows: list[bytes] = []
    for logical_row in range(height):
        stored_row = height - 1 - logical_row
        row = data[pixel_offset + stored_row * src_stride :
                   pixel_offset + (stored_row + 1) * src_stride]
        if xor_value:
            row = bytes(value ^ xor_value for value in row)
        rows.append(row)
    return b"".join(rows), width, height


def _preshift_row(source: bytes, bytes_per_row: int, shift: int) -> bytes:
    # This is the bmp2arr saveout_write_sprite loop expressed with explicit
    # 32-bit masking.  For the TH04 8-pixel sprites it reads one source byte
    # and emits the two-byte preshift window used by the original .asp files.
    shifted = 0
    output: list[int] = []
    remaining = bytes_per_row
    source_index = 0
    while remaining >= 2:
        remaining -= 1
        shifted = ((shifted << 8) + (source[source_index] << (8 - shift))) & 0xFFFFFFFF
        output.append((shifted >> 8) & 0xFF)
        source_index += 1
    while remaining >= 1:
        remaining -= 1
        shifted = (shifted << 8) & 0xFFFFFFFF
        output.append((shifted >> 8) & 0xFF)
    return bytes(output)


def _binary(value: int) -> str:
    return f"{value:08b}b"


def _asset_bytes(pixels: bytes, width: int, height: int,
                 spec: SpriteAsset) -> list[tuple[int, int, list[bytes]]]:
    source_bytes_per_row = (spec.sprite_width + 7) // 8
    output_bytes_per_row = source_bytes_per_row + (1 if spec.preshift else 0)
    sheet_cols = width // spec.sprite_width
    sheet_rows = height // spec.sprite_height
    if sheet_cols < 1 or sheet_rows < 1 or width % spec.sprite_width or height % spec.sprite_height:
        raise ValueError(f"sprite sheet geometry mismatch: {spec.input_path}")

    cells: list[tuple[int, int, list[bytes]]] = []
    source_row_stride = (((width + 7) // 8) + 3) & ~3
    # bmp2arr's outer preshift is [shift][cell][row], while inner preshift is
    # [cell][shift][row].  Preserve that distinction in the generated source.
    shifts = range(8) if spec.preshift else (0,)
    if spec.preshift == "outer":
        order = ((shift, sheet_row, sheet_col)
                 for shift in shifts
                 for sheet_row in range(sheet_rows)
                 for sheet_col in range(sheet_cols))
    else:
        order = ((shift, sheet_row, sheet_col)
                 for sheet_row in range(sheet_rows)
                 for sheet_col in range(sheet_cols)
                 for shift in shifts)
    for shift, sheet_row, sheet_col in order:
        sprite_number = sheet_row * sheet_cols + sheet_col
        encoded_rows: list[bytes] = []
        for row in range(spec.sprite_height):
            source_row = sheet_row * spec.sprite_height + row
            start = source_row * source_row_stride + sheet_col * source_bytes_per_row
            source = pixels[start : start + source_bytes_per_row]
            if len(source) != source_bytes_per_row:
                raise ValueError(f"sprite row truncated: {spec.input_path}")
            encoded_rows.append(
                _preshift_row(source, output_bytes_per_row, shift)
                if spec.preshift else source
            )
        cells.append((sprite_number, shift, encoded_rows))
    return cells


def generate_sprite_sources(root: Path, output_dir: Path) -> tuple[list[Path], list[dict[str, object]]]:
    """Generate private ``_DATA`` assembly owners and return paths/metadata."""
    output_dir.mkdir(parents=True, exist_ok=True)
    sources: list[Path] = []
    records: list[dict[str, object]] = []
    for spec in SPRITE_ASSETS:
        input_path = root / spec.input_path
        raw = input_path.read_bytes()
        pixels, width, height = _load_bmp_1bpp(root, spec)
        cells = _asset_bytes(pixels, width, height, spec)
        path = output_dir / f"{spec.symbol[1:].lower()}.asm"
        lines = [
            "; Private bmp2arr-compatible source generated for the native link diagnostic.",
            f"; Input: {spec.input_path}",
            f"; Input SHA-256: {_sha256(raw)}",
            ".386",
            ".model use16 large",
            "locals",
            "_DATA segment word public 'DATA' use16",
            "assume ds:_DATA",
            f"public {spec.symbol}",
            f"{spec.symbol} label byte",
        ]
        shifts = 8 if spec.preshift else 1
        for cell_index, shift, rows in cells:
            if spec.preshift:
                lines.append(f"; sprite {cell_index} preshift {shift}")
            for row_index, encoded in enumerate(rows):
                values = ",".join(_binary(value) for value in encoded)
                lines.append(f"db {values} ; row {row_index}")
        lines.extend(["_DATA ends", "end"])
        path.write_text("\n".join(lines) + "\n", encoding="ascii")
        sources.append(path)
        preshift_count = 8 if spec.preshift else 1
        records.append({
            "symbol": spec.symbol,
            "input": spec.input_path,
            "input_sha256": _sha256(raw),
            "expected_input_sha256": spec.expected_sha256,
            "source": path.relative_to(output_dir.parent).as_posix(),
            "source_sha256": _sha256(path.read_bytes()),
            "sprite_width": spec.sprite_width,
            "sprite_height": spec.sprite_height,
            "preshift": spec.preshift,
            "sprite_cells": len(cells) // preshift_count,
            "generated_chunks": len(cells),
            "preshift_count": preshift_count,
        })
    return sources, records
