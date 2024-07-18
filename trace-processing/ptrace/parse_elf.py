from elftools.elf.elffile import ELFFile
from elftools.dwarf.descriptions import describe_form_class
import os

from utils.utils import read_xml, terminal_colors, save_file_text, confirm
from pathlib import Path


class CFBlock:
    block_start = -1
    block_end = -1
    file_name = ""
    line_start = -1
    line_end = -1
    function_name = ""
    block_id = -1
    code = ""

    def __init__(self, block_start, block_end, file_name, line_start, line_end, function_name, code_raw,
                 code_file_index):
        self.block_start = block_start
        self.block_end = block_end
        self.file_name = file_name
        self.line_start = line_start
        self.line_end = line_end
        self.function_name = function_name
        self.code_file_index = code_file_index

        code_escaped = code_raw.translate(str.maketrans({"<": r"\<",
                                                         ">": r">]",
                                                         "\\": r"\\",
                                                         "^": r"\^",
                                                         '"': r'\"',
                                                         "&": r"\&",
                                                         "'": r'\"'}))  # TODO might have to fix this
        self.code = code_escaped

    def show(self):
        print(
            f"[BLOCK] ({hex(self.block_start)} -> {hex(self.block_end)})\n  {self.file_name}::{self.line_start} -> {self.line_end} ({self.function_name})\n--------\n{self.code}\n--------")

    def print_range(self):
        print(f"[BLOCK] ({hex(self.block_start)} -> {hex(self.block_end)})\n")


class Function:
    """ representing a function (needed by parsing elf) """
    name = ''
    start = -1
    end = -1

    def __init__(self, name, start, end):
        self.name = name
        self.start = start
        self.end = end


class CodeLine:
    line = -1
    pc = -1
    # how often this codeLine got hit by the run
    hitCount = 0

    def __init__(self, line, pc):
        self.line = line
        self.pc = pc


class CodeFile:
    """ representing a codefile (needed by parsing elf) """
    # id -> will be the index of compilation unit in elf file
    file_id = -1
    # name of codefile (e.g. with .cpp)
    name = ''
    # complete path (without name)
    path = ''
    # complete path with name
    path_name = ''
    # list of Function
    functions = []

    # list of all the compiled lines in this code file (in int)
    compiled_lines = []

    def __init__(self, file_id, name, path, path_name):
        self.file_id = file_id
        self.name = name
        self.path = path
        self.path_name = path_name


def convert_blocks_to_json(cfgblocks):
    json_string = '  "run_blocks": [\n'

    run_id = 0

    for run in cfgblocks:

        json_string += '    {\n'
        json_string += f'      "run_id": {run_id},\n'
        json_string += f'      "block": [\n'
        # TODO: block_start etc should be deleted (keep for now for testing correctness) (delete everything but block_id)
        for block in run:
            json_string += '        {\n'
            json_string += f'          "block_id": {block.block_id}\n'
            # json_string += f'          "block_start": {block.block_start},\n'
            # json_string += f'          "block_start_hex": "{hex(block.block_start)}",\n'
            # json_string += f'          "block_end": {block.block_end},\n'
            # json_string += f'          "block_end_hex": "{hex(block.block_end)}",\n'
            # json_string += f'          "file_name": "{block.file_name}",\n'
            # json_string += f'          "code_file_id": {block.code_file_index},\n'
            # json_string += f'          "line_start": {block.line_start},\n'
            # json_string += f'          "line_end": {block.line_end},\n'
            # json_string += f'          "function_name": "{block.function_name}"\n'
            json_string += '        },\n'

        json_string = json_string[:-2]
        json_string += '\n'
        json_string += '      ]\n'
        json_string += '    },\n'
        run_id += 1

    json_string = json_string[:-2]
    json_string += '\n'

    json_string += '  ],\n'

    return json_string


def convert_uniqueBlock_to_json(unique_blocks):
    json_string = '  "cfg_blocks": [\n'

    for block in unique_blocks:
        json_string += '        {\n'
        json_string += f'          "block_id": {block.block_id},\n'
        json_string += f'          "block_start": {block.block_start},\n'
        json_string += f'          "block_start_hex": "{hex(block.block_start)}",\n'
        json_string += f'          "block_end": {block.block_end},\n'
        json_string += f'          "block_end_hex": "{hex(block.block_end)}",\n'
        json_string += f'          "file_name": "{block.file_name}",\n'
        json_string += f'          "code_file_id": {block.code_file_index},\n'
        json_string += f'          "line_start": {block.line_start},\n'
        json_string += f'          "line_end": {block.line_end},\n'
        json_string += f'          "function_name": "{block.function_name}"\n'
        json_string += '        },\n'

    json_string = json_string[:-2]
    json_string += '\n'

    json_string += '  ],\n'

    return json_string


def convert_code_files_to_json(code_files):
    """ parses list of codefiles into json string and returns it"""

    json_string = '  "code_files": [\n'
    for code_file in code_files:
        json_string += '    {\n'
        json_string += f'      "file_id": {code_file.file_id},\n'
        json_string += f'      "name": "{code_file.name}",\n'
        json_string += f'      "path": "{code_file.path}",\n'
        json_string += f'      "path_name": "{code_file.path_name}",\n'
        json_string += '      "functions": [\n'
        for function in code_file.functions:
            json_string += '        {\n'
            json_string += f'          "name": "{function.name}",\n'
            json_string += f'          "start": {function.start},\n'
            json_string += f'          "start_hex": "{hex(function.start)}",\n'
            json_string += f'          "end": {function.end},\n'
            json_string += f'          "end_hex": "{hex(function.end)}"\n'
            json_string += '        },\n'
        if code_file.functions:
            json_string = json_string[:-2]
            json_string += '\n'
        json_string += '      ],\n'

        json_string += '      "compiled_code_lines": [\n'
        for compLine in code_file.compiled_lines:
            json_string += '        {\n'
            json_string += f'          "line": {compLine.line},\n'
            json_string += f'          "pc": {compLine.pc},\n'
            json_string += f'          "pc_hex": "{hex(compLine.pc)}",\n'
            json_string += f'          "hit_count": {compLine.hitCount}\n'
            json_string += '        },\n'
        if code_file.compiled_lines:
            json_string = json_string[:-2]
            json_string += '\n'
        json_string += '      ]\n'
        json_string += '    },\n'

    if code_files:
        json_string = json_string[:-2]
        json_string += '\n'
    json_string += '  ]\n'
    return json_string


# TODO: put this function below under to_json stuff
# TODO: give cfg_blocks_sorted as reference
# TODO: fix this!!!
def blocks_into_ID_list(cfg_blocks_sorted):
    """ goes through cfg_blocks_sorted puts unique blocks into list with IDs and in cfg_blocks_sorted ids are added to each block """
    unique_cfgBlocks = []
    myCfg_blocks_sorted = cfg_blocks_sorted
    unique_cfgBlocks_id = 0
    for run in myCfg_blocks_sorted:
        print("in for run")
        for block in run:
            print("in for block")
            blockContained = False
            containedId = -1
            if not unique_cfgBlocks:
                blockContained = False
            for unique_cfgBlock in unique_cfgBlocks:
                print(f"in for unique_cfgBlock: {unique_cfgBlocks_id}")
                if block.block_start == unique_cfgBlock.block_start and block.block_end == unique_cfgBlock.block_end:
                    blockContained = True
                    containedId = unique_cfgBlock.block_id
                    break
            if not blockContained:
                block.block_id = unique_cfgBlocks_id
                unique_cfgBlocks.append(block)
                unique_cfgBlocks_id += 1
            else:
                block.block_id = containedId

    return myCfg_blocks_sorted, unique_cfgBlocks


def convert_blocks_to_xml(cfgblocks):
    """ parses list of blocks into xml string and returns it"""
    xml = "<cfgblocks>\n"

    run_id = 0
    for run in cfgblocks:
        xml += f'<run id="{run_id}">\n'
        for block in run:
            xml += "<block "

            xml += f'block_start="{block.block_start}" '
            xml += f'block_end="{block.block_end}" '
            xml += f'file_name="{block.file_name}" '
            xml += f'line_start="{block.line_start}" '
            xml += f'line_end="{block.line_end}" '
            xml += f'function_name="{block.function_name}" '

            # xml += f"code='{block.code}'"
            xml += ">\n"

            # xml += block.code

            xml += "</block>\n"
        xml += "</run>\n"
        run_id += 1
    xml += "</cfgblocks>\n"
    return xml


def convert_code_files_to_xml(code_files):
    """ parses list of functions into xml string and returns it"""
    xml = "<functions>\n"
    for code_file in code_files:
        for function in code_file.functions:
            xml += f"<function name=\"{function.name}\" start=\"{function.start}\" end=\"{function.end}\">"
            xml += "</function>\n"

    xml += "</functions>\n"
    return xml


def read_source(path_c):
    """ reads the source code file """
    code_lines = []  # either open all files and search corresponding source file or just concat and use offset
    with open(path_c) as c_file:
        code_lines = c_file.readlines()
    # for line in code_lines:
    # print(line, end='')
    return code_lines


def decode_file_line(dwarfinfo, address):
    # Go over all the line programs in the DWARF information, looking for
    # one that describes the given address.
    compilation_unit_index = 0
    for CU in dwarfinfo.iter_CUs():
        # First, look at line programs to find the file/line for the address
        lineprog = dwarfinfo.line_program_for_CU(CU)

        prevstate = None
        for entry in lineprog.get_entries():
            # We're interested in those entries where a new state is assigned
            if entry.state is None:
                continue
            if entry.state.end_sequence:
                # if the line number sequence ends, clear prevstate.
                prevstate = None
                continue
            # Looking for a range of addresses in two consecutive states that
            # contain the required address.
            if prevstate and prevstate.address <= address < entry.state.address:
                filename = lineprog['file_entry'][prevstate.file - 1].name
                line = prevstate.line
                return filename, line, compilation_unit_index
            prevstate = entry.state
        compilation_unit_index += 1
    return None, None, None


def decode_funcname(dwarfinfo, address):
    # Go over all DIEs in the DWARF information, looking for a subprogram
    # entry with an address range that includes the given address. Note that
    # this simplifies things by disregarding subprograms that may have
    # split address ranges.
    for CU in dwarfinfo.iter_CUs():
        """attribs = CU.get_top_DIE().attributes
        #print(f"DF {CU.get_top_DIE().tag}")
        if('DW_FORM_line_strp' not in attribs):
            #print("missing attribute DW_FORM_line_strp")
            return None
        else:
            print("valid CU")"""
        for DIE in CU.iter_DIEs():
            try:
                if DIE.tag == 'DW_TAG_subprogram':
                    lowpc = DIE.attributes['DW_AT_low_pc'].value

                    # DWARF v4 in section 2.17 describes how to interpret the
                    # DW_AT_high_pc attribute based on the class of its form.
                    # For class 'address' it's taken as an absolute address
                    # (similarly to DW_AT_low_pc); for class 'constant', it's
                    # an offset from DW_AT_low_pc.
                    highpc_attr = DIE.attributes['DW_AT_high_pc']
                    highpc_attr_class = describe_form_class(highpc_attr.form)
                    if highpc_attr_class == 'address':
                        highpc = highpc_attr.value
                    elif highpc_attr_class == 'constant':
                        highpc = lowpc + highpc_attr.value
                    else:
                        print('Error: invalid DW_AT_high_pc class:',
                              highpc_attr_class)
                        continue

                    if lowpc <= address <= highpc:
                        function_name = DIE.attributes['DW_AT_name'].value
                        return function_name
            except KeyError:
                continue
    return None


def is_block_already_contained(block, block_list):
    """
    Return tuple (index of containing block, containment type) or (-1, -1) if not contained
    type:
    0 - fully enclosed
    1 - start before start (block contains c_block)
    2 - end after ending ERROR
    -1 - None

    """
    index = -1
    counter = 0
    occurences = 0
    for c_block in block_list:
        a_start = block.block_start
        a_end = block.block_end
        b_start = c_block.block_start
        b_end = c_block.block_end
        if (a_start == b_start and a_end == b_end):
            occurences += 1  # identical or duplicate
            if (occurences > 1):
                print("duplicate")
        else:
            if (a_start >= b_start and a_start <= b_end):  # block is inside current block
                # print(hex(a_start),hex(a_end),hex(b_start),hex(b_end))
                return (block_list.index(c_block), 0)  # end should always be identical
            else:
                if (a_start < b_start and a_end >= b_start):  # or block_end == cblock_end
                    # print(a_start,a_end,b_start,b_end)
                    return (block_list.index(c_block), 1)  # end should always be identical
        counter += 1
    if (occurences < 1):
        print("Warning block not found")
    return (-1, -1)


def split_into_blocks(path_elf, path_c, path_trace, runs, max_pc):
    # seems to be a set of blocks for each run -> to be returned by function
    cfblocks = []  # [run] [block]
    code_lines = read_source(path_c)

    tree, root = read_xml(path_trace)
    run = -1
    elf_addresses = set()
    # seems to be a set of blocks for each run
    blocks = [set() for _ in range(runs)]
    for entry in root:  # either parse trace or parse binary and follow binary execution through blocks
        if (entry.tag == "symex"):
            run += 1
            print(f"run {run}")
            # index 0 = execution trace
            block_start = -1  # int(entry[0][0].attrib.get('pc'))
            block_end = block_start
            # create the blocks (start normally and always end at jump or branch instruction)
            for child in entry[0]:
                step_attr = child.attrib
                pc_hex = step_attr.get('pc')
                pc = int(pc_hex, 16)
                if (block_start == -1):
                    block_start = pc  # test if pc == max_pc makes sense -> seems to be ok
                if (child[0].tag == "jump" or child[0].tag == "branch" or pc == max_pc):
                    block_end = pc
                    blocks[run].add((block_start, block_end))
                    block_start = -1
    # print(blocks)

    with open(path_elf, 'rb') as input_file:
        elf_file = ELFFile(input_file)

        if not elf_file.has_dwarf_info():
            print('file has no DWARF info')
            return

        dwarfinfo = elf_file.get_dwarf_info()
        last_file = ""
        for run in range(
                runs):  # this can create different blocks depending on run index, but this might be actually better
            # print(f"---- [RUN {run}] ----")
            current_block_list = []
            for block in blocks[run]:
                # print(f"block {block}")
                line_start = -1
                line_end = -1
                # print(f"{hex(block[0])} - {hex(block[1])}")
                file_1, line_start, code_file_index1 = decode_file_line(dwarfinfo, block[0])
                file_2, line_end, code_file_index2 = decode_file_line(dwarfinfo, block[1])
                funcname = decode_funcname(dwarfinfo, block[0])
                if (funcname):
                    funcname = funcname.decode("utf-8")
                else:
                    funcname = "Unknown"
                    print(f"[ERROR] Function name for code starting at line {line_start} unknown")

                # setting all the data we can
                if file_1:
                    file_1 = file_1.decode("utf-8")
                else:
                    file_1 = "Unknown"
                if line_start is None:
                    line_start = -1
                if line_end is None:
                    line_end = -1
                # this should only work if we assume a codeBlock always starts and ends in same codeFile
                # which should be the case, when not weird include stuff used I think
                # -> meaning one function that spans over multiple codeFiles
                if code_file_index2 is None:
                    code_file_index2 = -1
                if code_file_index1 is None:
                    code_file_index1 = code_file_index2
                # print('Function:', bytes2str(funcname))

                current_lines = ""
                current_block = CFBlock(block[0], block[1], file_1, line_start, line_end, funcname,
                                        current_lines[0:-1], code_file_index1)
                current_block_list.append(current_block)

                '''
                if (file_1 != None and file_2 != None and code_file_index != None):
                    file_1 = file_1.decode("utf-8")
                    file_2 = file_2.decode("utf-8")
                    if (last_file != file_1):
                        # print('\n[File]:', bytes2str(file_1))
                        last_file = file_1
                    current_lines = ""
                    for line in range(line_start, line_end):
                        # print(f'Line{line+1}: {code_lines[line][0:-1]}')#TODO is the line correct
                        current_lines += code_lines[line]
                    current_block = CFBlock(block[0], block[1], file_1, line_start, line_end, funcname,
                                            current_lines[0:-1], code_file_index)
                    current_block_list.append(current_block)
                else:
                    if (line_end is not None):
                        print(f"address {hex(line_end)} out of range")
                    else:
                        print(
                            f"{terminal_colors.WARNING}Unknown line numbers for block [{hex(block[0])} - {hex(block[1])}]{terminal_colors.ENDC}")
                '''
                # print()
            # print()

            # cleanup blocklist
            future_blocks_to_keep = []  # blocks not yet iterated over that contain a block to delete
            blocks_to_delete = []
            for block in current_block_list:
                if (block in future_blocks_to_keep or block in blocks_to_delete):
                    # print("block not processed")
                    continue
                else:
                    contained_index, contain_type = is_block_already_contained(block, current_block_list)
                    if (contained_index != -1):
                        # print("Block is already contained")
                        if (contain_type == 0):  # this block is fully contained in another
                            # block.print_range()
                            # print("is contained in")
                            # current_block_list[contained_index].print_range()
                            blocks_to_delete.append(block)
                            future_blocks_to_keep.append(current_block_list[contained_index])
                        if (contain_type == 1):  # this block contains a later one
                            # block.print_range()
                            # print("contains")
                            # current_block_list[contained_index].print_range()
                            blocks_to_delete.append(current_block_list[contained_index])
                    else:
                        pass
            merged_block_list = []
            for block in current_block_list:
                if (block not in blocks_to_delete):
                    merged_block_list.append(block)
                else:
                    # print("removed block")
                    # block.print_range()
                    pass

            cfblocks.append(merged_block_list)
    return cfblocks


def decode_code_files_function_ranges(path_elf):
    # functions = []  # (name,start,end,file)
    # holds all CodeFiles (datatype)
    code_files = []
    with open(path_elf, 'rb') as input_file:
        elf_file = ELFFile(input_file)

        if not elf_file.has_dwarf_info():
            print('file has no DWARF info')
            return

        compilation_unit_index = 0

        dwarfinfo = elf_file.get_dwarf_info()
        for CU in dwarfinfo.iter_CUs():

            # source https://github.com/eliben/pyelftools/blob/master/examples/dwarf_die_tree.py
            top_DIE = CU.get_top_DIE()
            # We're interested in the filename...
            path_name = Path(top_DIE.get_full_path()).as_posix()
            path, file = os.path.split(path_name)

            codeF = CodeFile(compilation_unit_index, file, path, path_name)
            functions = []

            """if(not CU):
                print("invalid CU")
                continue
            if(not CU.get_top_DIE()):
                print("invalid DIE")
                continue
            attribs = CU.get_top_DIE().attributes
            if('DW_FORM_line_strp' not in attribs):
                print(f"{CU.get_top_DIE().tag}")
                print("missing attribute DW_FORM_line_strp")
                continue"""
            for DIE in CU.iter_DIEs():
                try:
                    # print(DIE.tag)
                    # print(DIE.attributes['DW_AT_name'].value)
                    if DIE.tag == 'DW_TAG_subprogram':
                        lowpc = DIE.attributes['DW_AT_low_pc'].value

                        # DWARF v4 in section 2.17 describes how to interpret the
                        # DW_AT_high_pc attribute based on the class of its form.
                        # For class 'address' it's taken as an absolute address
                        # (similarly to DW_AT_low_pc); for class 'constant', it's
                        # an offset from DW_AT_low_pc.
                        highpc_attr = DIE.attributes['DW_AT_high_pc']
                        highpc_attr_class = describe_form_class(highpc_attr.form)
                        if highpc_attr_class == 'address':
                            highpc = highpc_attr.value
                        elif highpc_attr_class == 'constant':
                            highpc = lowpc + highpc_attr.value
                        else:
                            print('Error: invalid DW_AT_high_pc class:',
                                  highpc_attr_class)
                            continue

                        # filename = ""
                        func = Function(DIE.attributes['DW_AT_name'].value.decode("utf-8"), lowpc, highpc)
                        functions.append(func)

                        # functions.append((DIE.attributes['DW_AT_name'].value.decode("utf-8"), lowpc, highpc, filename))
                except KeyError:
                    continue
            codeF.functions = functions
            code_files.append(codeF)
            compilation_unit_index += 1
    # print(functions)
    return code_files


# compute for each codeFile all the compiled codeLines -> seems to work
def compute_compiled_code_lines(path_dir_elf, code_files):
    with open(path_dir_elf, 'rb') as input_file:
        elf_file = ELFFile(input_file)

        if not elf_file.has_dwarf_info():
            print('file has no DWARF info')
            return
        dwarfinfo = elf_file.get_dwarf_info()
        compilation_unit_index = 0
        for CU in dwarfinfo.iter_CUs():
            # First, look at line programs to find the file/line for the address
            lineprog = dwarfinfo.line_program_for_CU(CU)
            compCodeLines = []

            for entry in lineprog.get_entries():
                if entry.state is None:
                    continue
                print(
                    f"compilationUnitInd, Line: {compilation_unit_index}: , {entry.state.line}, {entry.state.address}")
                newCodeline = entry.state.line
                newPc = entry.state.address
                # we only want lowest pc for each codeLine (is sorted in order)
                if not (any(x.line == newCodeline for x in compCodeLines)):
                    compCodeLines.append(CodeLine(newCodeline, newPc))
                # if not (codeline in compCodeLines):
                #    compCodeLines.append(codeline)

            code_files[compilation_unit_index].compiled_lines = compCodeLines
            compilation_unit_index += 1

        index = 0
        for codefile in code_files:
            print(f"codeFilesIndex and compiled Lines: {index}, {codefile.compiled_lines}")


def compute_code_line_hits(path_trace, path_dir_elf, code_files):
    with open(path_dir_elf, 'rb') as input_file:
        elf_file = ELFFile(input_file)

        if not elf_file.has_dwarf_info():
            print('file has no DWARF info')
            return
        dwarfinfo = elf_file.get_dwarf_info()

        tree, root = read_xml(path_trace)

        previousCodeLine = -1
        previousCodeFileIndex = -1

        for entry in root:
            if (entry.tag == "symex"):
                for stp in entry[0]:
                    step_attr = stp.attrib
                    pc_hex = step_attr.get('pc')
                    # convert string from xml to int (in hex format)
                    pc = int(pc_hex, 16)
                    code_file, code_line, code_file_index = decode_file_line(dwarfinfo, pc)
                    print(f"codeFile: {code_file}, code_line: {code_line}, code_file_index: {code_file_index}")
                    if (code_line is not None) and (code_file_index is not None):
                        if previousCodeFileIndex != code_file_index or previousCodeLine != code_line:
                            previousCodeFileIndex = code_file_index
                            previousCodeLine = code_line
                            print("previoues set")
                            # search this line in our code_file and increase the hitCount
                            for codeFile in code_files:
                                if codeFile.file_id == code_file_index:
                                    print("codefileId is same")
                                    for codeLine in codeFile.compiled_lines:
                                        if codeLine.line == code_line:
                                            print("codeLine is same")
                                            codeLine.hitCount += 1
                                            print("hitCount increased")


def process_DWARF(path_dir_elf, path_dir_c, path_trace, output_path, num_runs, max_pc):
    # read_labels(path_dir_elf, path_dir_c, path_trace)
    path, file = os.path.split(path_trace)  # path_trace.split("/")[-1].split(".")[0]
    executable_name = file.split(".")[0]

    cfg_blocks = split_into_blocks(path_dir_elf, path_dir_c, path_trace, num_runs, max_pc)

    cfg_blocks_sorted = []
    for run in cfg_blocks:
        cfg_blocks_sorted.append(sorted(run, key=lambda b: b.block_start))

    # cfg_blocks_sorted is a list of list of cfgBlocks
    # goes through cfg_blocks_sorted puts unique blocks into list with IDs and in cfg_blocks_sorted ids are added to each block
    cfg_blocks_sorted, unique_blocks = blocks_into_ID_list(cfg_blocks_sorted)

    """for run in cfg_blocks_sorted:
        print("RUN ", cfg_blocks_sorted.index(run))
        for block in run:
            block.print_range()

    print("\n")"""

    # now just converting blk information to json and xml and writing it to file

    # converting blk information to json and writing to file

    json = '{\n'

    json += convert_blocks_to_json(cfg_blocks_sorted)
    json += convert_uniqueBlock_to_json(unique_blocks)
    code_files = decode_code_files_function_ranges(path_dir_elf)

    # compute for each codeFile all the compiled codeLines
    compute_compiled_code_lines(path_dir_elf, code_files)

    # compute for each codeLine how often it got hit by the original trace
    compute_code_line_hits(path_trace, path_dir_elf, code_files)

    json += convert_code_files_to_json(code_files)

    # json += convert_compiled_code_lines_to_json(code_files)

    json += '}'

    save_file_name = f"{output_path}/out_json.blk"
    if (os.path.isfile(save_file_name)):
        if (
                confirm(
                    f"BLK file [{save_file_name}] already exists. Overwrite? {terminal_colors.OKGREEN}[y/yes]{terminal_colors.ENDC} ")):
            save_file_text(json, save_file_name, True)
        else:
            print(f"{terminal_colors.WARNING}BLK file was not saved{terminal_colors.ENDC}")
    else:
        save_file_text(json, save_file_name, False)
        print(f"{terminal_colors.OKCYAN}Saved BLK file as [{save_file_name}]{terminal_colors.ENDC}")

    # converting blk information to xml and writing to file

    xml = '<?xml version="1.0" encoding="UTF-8"?>\n'
    xml += "<cfdata>\n"

    xml += convert_blocks_to_xml(cfg_blocks_sorted)
    # functions = decode_function_ranges(path_dir_elf)

    xml += convert_code_files_to_xml(code_files)

    xml += "</cfdata>\n"

    save_file_name = f"{output_path}/{executable_name}.blk"
    if (os.path.isfile(save_file_name)):
        if (
                confirm(
                    f"BLK file [{save_file_name}] already exists. Overwrite? {terminal_colors.OKGREEN}[y/yes]{terminal_colors.ENDC} ")):
            save_file_text(xml, save_file_name, True)
        else:
            print(f"{terminal_colors.WARNING}BLK file was not saved{terminal_colors.ENDC}")
    else:
        save_file_text(xml, save_file_name, False)
        print(f"{terminal_colors.OKCYAN}Saved BLK file as [{save_file_name}]{terminal_colors.ENDC}")
