"""
Generates a series of near-identical files where each file includes the next
one in the chain via a `.include "..."` directive. Useful for testing
recursive/chained include resolution (e.g. resolve_include in an assembler).

Just edit the variables below and run: python make_include_chain.py
"""

import os

# ----- CONFIG: edit these -----
DIRECTORY = "test"              # output directory (also used inside the .include path)
COUNT = 1                 # how many files to generate
PREFIX = "test"                 # filename prefix
EXT = "txt"                     # file extension (no dot)
START = 1                       # starting index
INCLUDE_FMT = '.include "{path}"'  # {path} = directory/filename of the NEXT file
ADD_INCLUDE_TO_LAST = False     # if True, last file includes back to the first (loop)

# The body of each generated file. Edit this freely — write it just like a
# normal file. Available placeholders: {index}, {total}, {include_line}
# ({include_line} is blank string on the last file, unless ADD_INCLUDE_TO_LAST).
TEMPLATE = """

// comments
// r0 - byte register
// wr0 - word register
// fr0 - float register



W_Array: .word {#1, #3, #5, #7};
Array: .byte {#1, #3, #5, #7};
player_two: Player {
    health : #1.4123,
    alive : #0,
    pos : Vec2{
        x : #1,
        y : #1,
    }
}

Small_Array: .allocate .byte[1024];
Big_Array: .allocate .word[1024];
player_one: .allocate Player;


String: .string "Burger\\n";

Label:
ADD r0 r2 r3;
LOAD r2 #0x0100;
STR [r0, r2] r2;
MOV fr0, wr2;

struct Vec2{
    x : .word,
    y : .word
};

struct Player{
    health : .float,
    alive : .byte,
    pos : Vec2
};



LOAD r2 [player_one, Player.pos.x];
{include_line}
"""
# --------------------------------


def build_body(index: int, total: int, include_line: str | None) -> str:
    """Content for a single file, filled in from TEMPLATE above.

    Uses plain .replace() instead of str.format() so that any literal
    curly braces you write in TEMPLATE (e.g. asm array/struct syntax like
    {#1, #3, #5, #7}) are left alone instead of being parsed as placeholders.
    """
    text = TEMPLATE
    text = text.replace("{index}", str(index))
    text = text.replace("{total}", str(total))
    text = text.replace("{include_line}", include_line or "")
    return text


def clear_existing_txt_files(directory: str):
    """Delete any .txt files already in the directory before regenerating."""
    if not os.path.isdir(directory):
        return
    for name in os.listdir(directory):
        if name.lower().endswith(".txt"):
            path = os.path.join(directory, name)
            if os.path.isfile(path):
                os.remove(path)
                print(f"deleted {path}")


def main():
    os.makedirs(DIRECTORY, exist_ok=True)
    clear_existing_txt_files(DIRECTORY)

    indices = list(range(START, START + COUNT))
    filenames = [f"{PREFIX}{i}.{EXT}" for i in indices]
    # path as it should appear INSIDE the .include directive
    include_paths = [f"{DIRECTORY}/{fname}" for fname in filenames]

    for pos, fname in enumerate(filenames):
        is_last = pos == len(filenames) - 1

        include_line = None
        if not is_last:
            next_path = include_paths[pos + 1]
            include_line = INCLUDE_FMT.format(path=next_path)
        elif ADD_INCLUDE_TO_LAST:
            include_line = INCLUDE_FMT.format(path=include_paths[0])

        body = build_body(indices[pos], len(filenames), include_line)
        out_path = os.path.join(DIRECTORY, fname)
        with open(out_path, "w", encoding="utf-8") as f:
            f.write(body)

        print(f"wrote {out_path}" + (f"  -> includes {include_line}" if include_line else ""))

    print(f"\nDone. {len(filenames)} files written to '{DIRECTORY}/'.")


if __name__ == "__main__":
    main()