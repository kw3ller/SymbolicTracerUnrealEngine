import argparse

from ptrace.parse_elf import process_DWARF
from ptrace.process_trace import process_trace_file

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Processes the raw SymEx trace file and outputs a PTrace file. ' +
                                                 'If the path to the binary and source file are specified also creates a .blk file')

    # Required positional argument
    parser.add_argument('--trace', type=str, required=True,
                        help='Path to raw SymEx trace file')

    parser.add_argument('--output', type=str, required=True,
                        help='Output directory')
    parser.add_argument('--elf', type=str, required=False,
                        help='Path to ELF binary')
    parser.add_argument('--source', type=str, required=False,
                        help='Path to source file')

    args = parser.parse_args()

    num_runs, max_pc = process_trace_file(args.trace, args.output, args.elf)

    if (args.elf and args.source):
        print("ELF and Source path specified. Creating optional .blk file.")
        process_DWARF(args.elf, args.source, args.trace, args.output, num_runs, max_pc)
