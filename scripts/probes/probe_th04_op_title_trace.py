#!/usr/bin/env python3
"""Cold-build an OP title trace overlay and restore every product input."""

from __future__ import annotations

import argparse
import hashlib
from pathlib import Path
import subprocess
import sys


ROOT = Path(__file__).resolve().parents[2]
SOURCES = {
    "src/op/title/op_animate.cpp": "38c9b93498a50483df5fc932a6cd57d8b3d65bf21318c8650d9f9accb3c9b71f",
    "src/op/title/op_animate.inl": "ae62337c40d4f5d61612b6089e81219891d138339f9478b8084077ed602d721f",
    "src/op/main/main.inl": "a613ed161df9b64be38d99f8bc018c75e9d98e665b564f37abfb3a51d719f897",
    "src/op/main/main_update_and_render.inl": "99f93845a8cd1c6f56aab37d5b387a580981a9305f433e7c5c0973d9bb961188",
    "src/op/main/menu.cpp": "f55ec5354a7e898b42854918d72961190185e110ba2cb0181a063ed2767ad0ec",
    "src/op/main/main_unput_and_put.inl": "61ffbe0c234514fd75b9534768508022d6b2325ef9023f6d11f9f54227a7dc5a",
}


def replace_once(data: bytes, before: str, after: str) -> bytes:
    old = before.encode("ascii")
    if data.count(old) != 1:
        raise ValueError(f"expected one trace insertion point: {before!r}")
    return data.replace(old, after.encode("ascii"))


def overlay() -> dict[str, bytes]:
    title = (ROOT / "src/op/title/op_animate.cpp").read_bytes()
    title = replace_once(title, '#include "src/shared/platform/types.hpp"',
                         '#include <dos.h>\n#include "src/shared/formats/cdg.hpp"\n#include "src/shared/platform/types.hpp"')
    title = replace_once(title, '#pragma codeseg OP_NATIVE_TEXT OP_NATIVE_01\n#include',
                         '''#pragma codeseg OP_NATIVE_TEXT OP_NATIVE_01
void near op_diag_mark(char code)
{
    static char last;
    int handle;
    unsigned done;
    if(code <= last) return;
    last = code;
    if(_dos_creat("OPMARK.TXT", 0, &handle) == 0) {
        _dos_write(handle, &code, 1, &done);
        _dos_close(handle);
    }
}
void near op_diag_dump(void)
{
    int handle;
    unsigned done;
    if(_dos_creat("OPCDG.BIN", 0, &handle) == 0) {
        _dos_write(handle, &cdg_slots[10], 16, &done);
        _dos_write(handle, &cdg_slots[35], 16, &done);
        _dos_write(handle, &cdg_slots[36], 16, &done);
        _dos_close(handle);
    }
}
#include''')

    animation = (ROOT / "src/op/title/op_animate.inl").read_bytes()
    insertions = (
        ('\t// Flash', "\top_diag_mark('A');\n\t// Flash"),
        ('\tif(resident->demo_num == 0) {',
         "\top_diag_mark('B');\n\tif(resident->demo_num == 0) {"),
        ('\tgraph_accesspage(1);\n\tpi_fullres_load_palette_apply_put_free',
         "\top_diag_mark('C');\n\tgraph_accesspage(1);\n\tpi_fullres_load_palette_apply_put_free"),
        ('\tgraph_copy_page(0);\n\n\t// Black-and-white fade-in',
         "\tgraph_copy_page(0);\n\top_diag_mark('D');\n\n\t// Black-and-white fade-in"),
        ('\t// Fade to the .PI palette.',
         "\top_diag_mark('E');\n\t// Fade to the .PI palette."),
        ('\tpi_palette_apply(0);\n}',
         "\tpi_palette_apply(0);\n\top_diag_mark('F');\n}"),
    )
    for before, after in insertions:
        animation = replace_once(animation, before, after)

    main = (ROOT / "src/op/main/main.inl").read_bytes()
    for before, after in (
        ('void main(void)',
         'void near op_diag_mark(char code);\nvoid near op_diag_dump(void);\n\nvoid main(void)'),
        ('\top_animate();', "\top_diag_mark('0');\n\top_animate();\n\top_diag_mark('G');"),
        ('\tcleardata_and_regist_view_sprites_load();\n\tmain_cdg_load();\n#endif',
         "\tcleardata_and_regist_view_sprites_load();\n\top_diag_mark('H');\n\tmain_cdg_load();\n\top_diag_mark('I');\n#endif"),
        ('\twhile(!quit) {', "\top_diag_mark('J');\n\twhile(!quit) {"),
        ('\t\tinput_reset_sense_interface();\n\t\tswitch(in_option)',
         "\t\tinput_reset_sense_interface();\n\t\top_diag_mark('K');\n\t\tswitch(in_option)"),
        ('\t\t\tmain_update_and_render();\n\t\t\tif(idle_frame',
         "\t\t\top_diag_mark('L');\n\t\t\tmain_update_and_render();\n\t\t\top_diag_mark('h');\n\t\t\tif(idle_frame"),
        ('\t\tframe_delay(1);\n\t}\n\tmain_cdg_free();',
         "\t\top_diag_mark('i');\n\t\tframe_delay(1);\n\t\top_diag_mark('j');\n\t}\n\tmain_cdg_free();"),
    ):
        main = replace_once(main, before, after)
    main = replace_once(main, "\tmain_cdg_load();\n\top_diag_mark('I');",
                        "\tmain_cdg_load();\n\top_diag_dump();\n\top_diag_mark('I');")
    menu_update = (ROOT / "src/op/main/main_update_and_render.inl").read_bytes()
    for before, after in (
        ('void near main_update_and_render(void)',
         'void near op_diag_mark(char code);\n\nvoid near main_update_and_render(void)'),
        ('\tstatic bool input_allowed;\n\n\tif(!initialized)',
         "\tstatic bool input_allowed;\n\top_diag_mark('M');\n\n\tif(!initialized)"),
        ('\t\tmenu_init(\n', "\t\top_diag_mark('N');\n\t\tmenu_init(\n"),
        ('\t\t);\n\t}\n\n\tif(!key_det)',
         "\t\t);\n\t\top_diag_mark('g');\n\t}\n\n\tif(!key_det)"),
    ):
        menu_update = replace_once(menu_update, before, after)
    menu = (ROOT / "src/op/main/menu.cpp").read_bytes()
    for before, after in (
        ('#define menu_init(initialized, allowed, count, render, left, width, bottom)',
         'void near op_diag_mark(char code);\n#define menu_init(initialized, allowed, count, render, left, width, bottom)'),
        ('    allowed = false; \\\n    egc_copy_rect_1_to_0_16(left, MENU_TOP, (width) + 32, \\',
         "    allowed = false; \\\n    op_diag_mark('O'); \\\n    egc_copy_rect_1_to_0_16(left, MENU_TOP, (width) + 32, \\"),
        ('                             (bottom) + 24 - MENU_TOP); \\\n    for(int i = 0; i < (count); i++) { \\',
         "                             (bottom) + 24 - MENU_TOP); \\\n    op_diag_mark('P'); \\\n    for(int i = 0; i < (count); i++) { \\"),
        ('        render(i, (menu_sel == i) ? COL_ACTIVE : COL_INACTIVE); \\',
         "        render(i, (menu_sel == i) ? COL_ACTIVE : COL_INACTIVE); \\\n        op_diag_mark((char)('a' + i)); \\"),
    ):
        menu = replace_once(menu, before, after)
    menu_render = (ROOT / "src/op/main/main_unput_and_put.inl").read_bytes()
    for before, after in (
        ('\tscreen_y_t top = main_choice_top(sel);',
         "\tscreen_y_t top = main_choice_top(sel);\n\top_diag_mark('Q');"),
        ('\tgrcg_setcolor(GC_RMW, col);',
         "\top_diag_mark('R');\n\tgrcg_setcolor(GC_RMW, col);\n\top_diag_mark('S');"),
        ('\t}\n\tgrcg_off();\n\n\tif(col == COL_ACTIVE)',
         "\t}\n\top_diag_mark('T');\n\tgrcg_off();\n\top_diag_mark('U');\n\n\tif(col == COL_ACTIVE)"),
        ('\t\tcdg_put_8(COMMAND_CURSOR_LEFT_LEFT,  top, CDG_CURSOR_LEFT);',
         "\t\tcdg_put_8(COMMAND_CURSOR_LEFT_LEFT,  top, CDG_CURSOR_LEFT);\n\t\top_diag_mark('V');"),
        ('\t\tcdg_put_8(COMMAND_CURSOR_RIGHT_LEFT, top, CDG_CURSOR_RIGHT);',
         "\t\tcdg_put_8(COMMAND_CURSOR_RIGHT_LEFT, top, CDG_CURSOR_RIGHT);\n\t\top_diag_mark('W');"),
        ('\t\tdesc_unput_and_put(desc_id);\n\t}\n}',
         "\t\tdesc_unput_and_put(desc_id);\n\t\top_diag_mark('X');\n\t}\n\top_diag_mark('Y');\n}"),
    ):
        menu_render = replace_once(menu_render, before, after)
    return {
        "src/op/title/op_animate.cpp": title,
        "src/op/title/op_animate.inl": animation,
        "src/op/main/main.inl": main,
        "src/op/main/main_update_and_render.inl": menu_update,
        "src/op/main/menu.cpp": menu,
        "src/op/main/main_unput_and_put.inl": menu_render,
    }


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output-dir", type=Path, required=True)
    args = parser.parse_args()
    original = {name: (ROOT / name).read_bytes() for name in SOURCES}
    for name, data in original.items():
        if hashlib.sha256(data).hexdigest() != SOURCES[name]:
            raise ValueError(f"product source changed before trace build: {name}")
    traced = overlay()
    try:
        for name, data in traced.items():
            (ROOT / name).write_bytes(data)
        return subprocess.run(
            [sys.executable, str(ROOT / "scripts/probes/probe_th04_native_op_link.py"),
             "--without-support", "--output-dir", str(args.output_dir)],
            cwd=ROOT, check=False,
        ).returncode
    finally:
        for name, data in original.items():
            (ROOT / name).write_bytes(data)


if __name__ == "__main__":
    raise SystemExit(main())
