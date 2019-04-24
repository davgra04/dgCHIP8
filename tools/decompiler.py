import argparse
import re
from pprint import pprint, pformat

debug = False
# debug = True


def debug_print(m):
    if debug:
        print(m)


def binary_viz(data):
    viz = ""
    for i in reversed(range(16)):
        if i == 7:
            viz += " "

        if (data >> i) & 0x1:
            viz += "■"
            # viz += "▉"
        else:
            # viz += "□"
            viz += "_"
    return viz

def print_instruction(addr, instruction_bytes):

    instruction_int = int.from_bytes(instruction_bytes, byteorder="big")
    instruction = "{0:04x}".format(instruction_int)
    # [2:]

    debug_print(pformat(instruction))
    debug_print(type(instruction))

    # return

    instruction_str = ""
    instruction_desc = ""
    if instruction == "00e0":
        instruction_str = "CLS"
        instruction_desc = "Clear the display"
    elif instruction == "00ee":
        instruction_str = "RET"
        instruction_desc = "Return from subroutine"
    elif instruction[0] == "1":
        instruction_str = "JP {}".format(instruction[1:])
        instruction_desc = "Jump to address 0x{}".format(instruction[1:])
    elif instruction[0] == "2":
        instruction_str = "CALL {}".format(instruction[1:])
        instruction_desc = "Call subroutine at 0x{}".format(instruction[1:])
    elif instruction[0] == "3":
        instruction_str = "SE V{0}, {1}".format(instruction[1], instruction[2:])
        instruction_desc = "Skip next instruction if V{0} == {1}".format(instruction[1], instruction[2:])
    elif instruction[0] == "4":
        instruction_str = "SNE V{0}, {1}".format(instruction[1], instruction[2:])
        instruction_desc = "Skip next instruction if V{0} != {1}".format(instruction[1], instruction[2:])
    elif instruction[0] == "5":
        instruction_str = "SE V{0}, V{1}".format(instruction[1], instruction[2])
        instruction_desc = "Skip next instruction if V{0} == V{1}".format(instruction[1], instruction[2])
    elif instruction[0] == "6":
        instruction_str = "LD V{0}, {1}".format(instruction[1], instruction[2:])
        instruction_desc = "Store {1} in V{0}".format(instruction[1], instruction[2:])
    elif instruction[0] == "7":
        instruction_str = "ADD V{0}, {1}".format(instruction[1], instruction[2:])
        instruction_desc = "Add {1} to V{0}".format(instruction[1], instruction[2:])
    elif instruction[0] == "8":

        if instruction[3] == "0":
            instruction_str = "LD V{0}, V{1}".format(instruction[1], instruction[2])
            instruction_desc = "Store V{1} into V{0}".format(instruction[1], instruction[2])
        if instruction[3] == "1":
            instruction_str = "OR V{0}, V{1}".format(instruction[1], instruction[2])
            instruction_desc = "Set V{0} to V{0} OR V{1}".format(instruction[1], instruction[2])
        if instruction[3] == "2":
            instruction_str = "AND V{0}, V{1}".format(instruction[1], instruction[2])
            instruction_desc = "Set V{0} to V{0} AND V{1}".format(instruction[1], instruction[2])
        if instruction[3] == "3":
            instruction_str = "XOR V{0}, V{1}".format(instruction[1], instruction[2])
            instruction_desc = "Set V{0} to V{0} XOR V{1}".format(instruction[1], instruction[2])
        if instruction[3] == "4":
            instruction_str = "ADD V{0}, V{1}".format(instruction[1], instruction[2])
            instruction_desc = "Set V{0} to V{0} + V{1}, Vf = carry".format(instruction[1], instruction[2])
        if instruction[3] == "5":
            instruction_str = "SUB V{0}, V{1}".format(instruction[1], instruction[2])
            instruction_desc = "Set V{0} to V{0} - V{1}, Vf = NOT borrow".format(instruction[1], instruction[2])
        if instruction[3] == "6":
            instruction_str = "SHR V{0}, V{1}".format(instruction[1], instruction[2])
            instruction_desc = "Shift V{0} right by 1 bit, Vf = least significant bit of V{0} before shift".format(instruction[1])
        if instruction[3] == "7":
            instruction_str = "SUBN V{0}, V{1}".format(instruction[1], instruction[2])
            instruction_desc = "Set V{0} to V{1} - V{0}, Vf = NOT borrow".format(instruction[1], instruction[2])
        if instruction[3] == "E":
            instruction_str = "SHL V{0}, V{1}".format(instruction[1], instruction[2])
            instruction_desc = "Shift V{0} left by 1 bit, Vf = most significant bit of V{0} before shift".format(instruction[1])

    elif instruction[0] == "9":
        instruction_str = "SNE V{0}, {1}".format(instruction[1], instruction[2:])
        instruction_desc = "Skip next instruction if V{0} == V{1}".format(instruction[1], instruction[2:])
    elif instruction[0] == "a":
        instruction_str = "LD I, {0}".format(instruction[1:])
        instruction_desc = "Set value of register I to {0}".format(instruction[1:])
    elif instruction[0] == "b":
        instruction_str = "JP V{0}, {1}".format(instruction[1], instruction[2:])
        instruction_desc = "Jump to location V{0} + {1}".format(instruction[1], instruction[2:])
    elif instruction[0] == "c":
        instruction_str = "RND V{0}, {1}".format(instruction[1], instruction[2:])
        instruction_desc = "Set V{0} to random_byte AND {1}".format(instruction[1], instruction[2:])
    elif instruction[0] == "d":
        instruction_str = "DRW V{0}, V{1}, {2}".format(instruction[1], instruction[2], instruction[3])
        instruction_desc = "Display {2}-byte sprite starting at memory location I at (V{0}, V{1}), Vf = collision".format(instruction[1], instruction[2], instruction[3])
    elif instruction[0] == "e":

        if instruction[2:] == "9e":
            instruction_str = "SKP V{0}".format(instruction[1])
            instruction_desc = "Skip next instruction if key with value of V{0} is pressed".format(instruction[1])
        if instruction[2:] == "a1":
            instruction_str = "SKNP V{0}".format(instruction[1])
            instruction_desc = "Skip next instruction if key with value of V{0} is not pressed".format(instruction[1])

    elif instruction[0] == "f":

        if instruction[2:] == "07":
            instruction_str = "LD V{0}, delay_timer".format(instruction[1])
            instruction_desc = "Set V{0} to value of delay_timer".format(instruction[1])
        if instruction[2:] == "0a":
            instruction_str = "LD V{0}, key".format(instruction[1])
            instruction_desc = "Wait for key press, and set V{0} to value of pressed key".format(instruction[1])
        if instruction[2:] == "15":
            instruction_str = "LD delay_timer, V{0}".format(instruction[1])
            instruction_desc = "Set delay_timer to value of V{0}".format(instruction[1])
        if instruction[2:] == "18":
            instruction_str = "LD sound_timer, V{0}".format(instruction[1])
            instruction_desc = "Set sound_timer to value of V{0}".format(instruction[1])
        if instruction[2:] == "1e":
            instruction_str = "ADD I, V{0}".format(instruction[1])
            instruction_desc = "Set I to I + V{0}".format(instruction[1])
        if instruction[2:] == "29":
            instruction_str = "LD F, V{0}".format(instruction[1])
            instruction_desc = "Set I to location of sprite for digit V{0}".format(instruction[1])
        if instruction[2:] == "33":
            instruction_str = "LD B, V{0}".format(instruction[1])
            instruction_desc = "Store BCD representation of V{0} in memory locations I, I+1, and I+2".format(instruction[1])
        if instruction[2:] == "55":
            instruction_str = "LD [I], V{0}".format(instruction[1])
            instruction_desc = "Store registers V0 through V{0} into memory starting at location I".format(instruction[1])
        if instruction[2:] == "65":
            instruction_str = "LD V{0}, [I]".format(instruction[1])
            instruction_desc = "Read registers V0 through V{0} from memory starting at location I".format(instruction[1])

    print("0x{0:04x}    {1:04x}    {2:10s}    {3:18s}    // {4:s}".format(
        addr+0x200,
        instruction_int,
        binary_viz(instruction_int),
        instruction_str,
        instruction_desc))


parser = argparse.ArgumentParser(description="Decompile a CHIP8 program.")
parser.add_argument("infile", type=str, help="CHIP8 program to decompile")
args = parser.parse_args()

program = b""

with open(args.infile, "rb") as f:

    data = f.read(2)
    addr = 0
    while data:
        print_instruction(addr, data)
        data = f.read(2)
        addr += 2
