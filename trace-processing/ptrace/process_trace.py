import copy
import os

from elftools.elf.elffile import ELFFile
from elftools.dwarf.descriptions import describe_form_class
from data.instructions import Link_Node, Link_Data, Analysis_Data, Run, Arith_Instruction, Instruction, Jump, Branch, \
    LoadStore, CSR, UniqueStep, AnimationStep
from enums.riscv_enum import Reg, Symbolic_Beh, Opcode, Opcode_type
from ptrace.parse_elf import decode_file_line
from utils.utils import terminal_colors, read_xml, confirm, save_file_text

MAX_STEPS_TO_GENERATE = 10000  # don't create all steps for very large traces


def determine_symbolic_behavior(instr_data, reg_rs1, reg_rs2, reg_rd,
                                reg_rs1_symbolic, reg_rs2_symbolic, reg_rd_symbolic, rd_was_symbolic):
    """determines the symbolic behavior of the instruction"""
    if ("beh" in instr_data.attrib.keys()):
        beh = instr_data.attrib.get("beh")
        if beh == "none":
            return Symbolic_Beh.none
        elif beh == "destroy":
            return Symbolic_Beh.destroy
        elif beh == "update":
            return Symbolic_Beh.update
        elif beh == "create":
            return Symbolic_Beh.create
        elif beh == "overwrite":
            return Symbolic_Beh.overwrite
        elif beh == "special":
            return Symbolic_Beh.special
        elif beh == "error":
            return Symbolic_Beh.unknown
        else:
            print(f"[ERROR] unknown symbolic behavior for instruction {instr_data}")
            return Symbolic_Beh.unknown
    else:  # trace doesn't contain any information about symbolic behavior. Try to infer behavior from available data.
        if (rd_was_symbolic and not reg_rd_symbolic):
            return Symbolic_Beh.destroy

        # jan TODO add previous rd
        elif (not reg_rs1_symbolic and not reg_rs2_symbolic and not reg_rd_symbolic):
            return Symbolic_Beh.none

        elif (reg_rd_symbolic and not rd_was_symbolic):
            return Symbolic_Beh.create

        elif (reg_rd_symbolic and rd_was_symbolic):
            if (reg_rd == reg_rs1 or reg_rd == reg_rs2):
                return Symbolic_Beh.update
            else:
                return Symbolic_Beh.overwrite

        else:
            print("ERROR: unknown symbolic behavior")
            return Symbolic_Beh.unknown


def get_instr_reg(instr_data, attrib):
    """kw: return instruction register and if it is symbolic?"""
    register = Reg.none
    is_symbolic = False
    if (attrib in instr_data.attrib.keys()):
        register = Reg[instr_data.attrib.get(attrib).split()[0].split("/")[
            0]]  # TODO fix trace registernames so only one split is necessary
        is_symbolic = instr_data.attrib.get(attrib)[-1] == "S"
    return (register, is_symbolic)


def get_imm(instr_data, attrib):
    """kw: no idea..."""
    value = None
    if (attrib in instr_data.attrib.keys()):
        value = int(instr_data.attrib.get(attrib).split()[0].split("/")[0])
    return value


def update_callstack(call_stack, link_reg, link_address):
    """update callstack (depth) if opcode is a jump"""
    depth = len(call_stack)

    if (link_reg != "zero (x0)"):
        call_stack.append(link_address)
    else:
        if (depth > 0):
            expected_return_address = call_stack[depth - 1]
            if (link_address == expected_return_address):
                call_stack.pop()


def process_instruction(run, instr, pc, call_stack, reg_rs1, reg_rs2, reg_rd, imm):
    """determine stuff like the type of the instruction (e.g. store, arith, load etc.)"""

    instr_data = instr[0]

    opcode_string = ""
    if (instr_data.tag == "jump"):  # TODO maybe refactor this and add opcodename to jump trace data
        opcode_string = "JAL"
    elif (instr_data.tag == "ECALL"):
        opcode_string = "ECALL"
    else:
        opcode_string = instr_data.get('opcode')
    current_opcode = Opcode[opcode_string]

    # create instruction
    depth = len(call_stack)
    current_instruction = None
    if (instr_data.tag == "instruction"):
        current_instruction = Arith_Instruction(pc, run, current_opcode, depth=depth,
                                                reg_rs1=reg_rs1, reg_rs2=reg_rs2, reg_rd=reg_rd, imm1=imm)
    elif (instr_data.tag == "ECALL"):
        current_instruction = Instruction(pc, run, current_opcode, depth=depth, type=Opcode_type.ECALL)
    elif (instr_data.tag == "jump"):
        jump_target = int(instr_data.attrib.get("target"), 16)
        link_reg = instr_data.attrib.get("link")
        link_address = int(instr_data.attrib.get("link-address"), 16)

        current_instruction = Jump(pc, run, current_opcode, depth=depth, target=jump_target,
                                   link_reg=Reg[link_reg.split()[0].split("/")[0]], link_address=link_address)

        update_callstack(call_stack, link_reg, link_address)

    elif (instr_data.tag == "branch"):
        jump_target = int(instr_data.attrib.get("target"), 16)  # int(entry[0][step+1].get('pc'),16)
        branch_edge = int(instr_data.attrib.get("cond"))
        current_instruction = Branch(pc, run, current_opcode, depth=depth, target=jump_target,
                                     reg_rs1=reg_rs1, reg_rs2=Reg.none, condition=branch_edge > 0)

    elif (instr_data.tag == "load" or instr_data.tag == "store"):
        current_instruction = LoadStore(pc, run, current_opcode, target=int(instr_data.attrib.get("target")[2:], 16),
                                        # TODO remove 0x from raw trace
                                        reg_rs1=reg_rs1, imm1=None, reg_rs2d=Reg.none, depth=depth)
    elif (instr_data.tag == "csr"):
        current_instruction = CSR(pc, run, current_opcode, -1, depth=depth)
    else:
        current_instruction = Instruction(pc, run, Opcode.ERROR, depth)
        print("ERROR: Unknown opcode")
    return current_instruction


def analyse_trace(root):
    """Perform basic analysis on the trace to be able to detect any large gaps in memory/code later and improve visualization"""
    global_start = 0
    # maximum program counter (init to min)
    max_pc = 0
    # minimum program counter (init to max)
    min_pc = 0xFFFFFF

    # number of all runs
    num_runs = 0
    # list of where all runs start: (node.run_id, node.data.pc, node.data.step)
    run_start = []
    # all timeliness entries as own list for each run (list of list)
    potential_child_branches = []

    timeline_forks = [0]  # TODO remove (never gets used/changed)

    memory_list = []
    # lists sets of every run with load store instruction targets
    memory_list_per_run = []

    global_start = int(root[0][0][0].attrib.get('pc'), 16)  # NOTE Jan: Handle different note types?
    # set of targets from all load and store instructions from all runs (access of memory)
    temp_memory_access_set = set()
    # number of all memory accesses
    total_accesses = 0

    for entry in root:
        if (entry.tag == "symex"):
            # this is a new run
            num_runs += 1

            # set of targets from all load and store instructions from this run (access of memory)
            memory_access_current_run = set()
            for stp in entry[0]:
                step_attr = stp.attrib
                pc_hex = step_attr.get('pc')
                # convert string from xml to int (in hex format)
                pc = int(pc_hex, 16)

                # possibly set new max and min program counters (I guess just for visualization later in blender)
                if (pc > max_pc):
                    max_pc = pc
                if (pc < min_pc):
                    min_pc = pc

                # check which memory regions are accessed to create a compressed view later
                # add memory targets if load or store instruction
                if (stp[0].tag == "load" or stp[0].tag == "store"):
                    # convert target to normal integer # NOTE: why? -> maybe easier calculations
                    memory_target = int(stp[0].attrib.get('target'), 16)
                    temp_memory_access_set.add(memory_target)
                    memory_access_current_run.add(memory_target)
                    total_accesses += 1
            memory_list_per_run.append(list(memory_access_current_run))
        if (entry.tag == "timelines"):
            tree = Link_Node(None, -1, None)  # root
            for id, run_i in enumerate(entry):
                link_data = []
                current_node = tree  # for each path/run start at the root
                for branch in run_i:
                    # program counter again to normal int ?!
                    branch_pc = int(branch.attrib.get('pc'), 16)
                    branch_step = int(branch.attrib.get('step'))
                    branch_hash = int(branch.attrib.get('hash'))
                    p_data = Link_Data(branch_hash, branch_pc, branch_step)
                    link_data.append(p_data)
                    current_node = current_node.insert(p_data, id, 1)
                # adding all timeliness entries as own list for each run
                potential_child_branches.append(link_data)
            # print(f"tree: {tree.PrintTree()}")
            # print(tree.to_xml())
            tree.display()
            run_start.append((-1, 0, 0))
            # collect all run starts
            for r in range(1, num_runs):
                node = tree.discover_run_start(r)
                if (node):
                    run_start.append((node.run_id, node.data.pc, node.data.step))
                else:
                    print(
                        f"{terminal_colors.FAIL}[ERROR] missing branch data for run {terminal_colors.OKCYAN}{r}{terminal_colors.ENDC}")
            print(run_start)
    # just for printing I guess
    memory_list = list(temp_memory_access_set)
    memory_list.sort()
    print(
        f"{terminal_colors.OKCYAN}Accessed Memory[{total_accesses} -> {len(temp_memory_access_set)}]: {len(memory_list)}{terminal_colors.ENDC}")

    # create parent child run list -> add empty list for each run
    discovered_run_links = []
    for run_id_n in range(num_runs):
        discovered_run_links.append([])

    # I don't quite understand this
    for run_id in range(num_runs):
        # if(run_id==0): # all but the first run must have parent runs
        # reached_start= True
        # continue

        # child_id is the id from the list run_start and link is the first element of list item in list run start ->
        # run_id
        for child_id, link in enumerate(run_start):

            if (run_id == link[0]):
                print(f"Child run with id {child_id} found for parent {run_id}")
                discovered_run_links[run_id].append((child_id, run_start[child_id][1], run_start[child_id][2]))
                break

    # collect and return all analysis results
    analysis_results = Analysis_Data(global_start, min_pc, max_pc, num_runs, timeline_forks, run_start,
                                     potential_child_branches, memory_list, memory_list_per_run)
    analysis_results.discovered_run_links = discovered_run_links
    return analysis_results


def process_trace(root, analysis_data):
    run_start = analysis_data.run_start
    print(
        f"{terminal_colors.HEADER}Processing trace: (with input analysis data [{len(analysis_data.memory_list)}]){terminal_colors.ENDC}")

    run_list = []
    run = 0

    for entry in root:
        print(f"Found entry [{entry.tag}]")
        if (entry.tag == "symex"):
            # a run was found
            run_entry = entry[0]
            run += 1
            print(f"-> {terminal_colors.BOLD}[Processing run:{run - 1}]{terminal_colors.ENDC}")
            # depth = 0
            call_stack = []

            # get start pc and start_step (from xml)
            start = int(run_entry[0].attrib.get('pc'), 16)
            start_step = int(run_entry[0].attrib.get('step'))

            # collect these for each run
            parent_id = -1
            branch_pc = start
            branch_step = start_step
            if (run_start):
                if (len(run_start) >= run):
                    parent_id = run_start[run - 1][0]
                    branch_pc = run_start[run - 1][1]
                    branch_step = run_start[run - 1][2]
                else:
                    print(
                        f"{terminal_colors.FAIL}[ERROR] missing parent analysis data for run {terminal_colors.OKCYAN}{run - 1}{terminal_colors.ENDC}")
            else:
                print(f"{terminal_colors.FAIL}[ERROR] missing run start analysis data for run{terminal_colors.ENDC}")

            current_run = Run(run, start)
            current_run.parent_id = parent_id
            pc_end = int(run_entry[-1].attrib.get('pc'), 16)
            current_run.end = pc_end

            step_counter = -1
            reached_start = False

            # print(f"Number of memory accesses: {len(memory_list)}")

            # we now iterate through each step in each entry
            for instr in run_entry:
                step_counter += 1
                if (step_counter > MAX_STEPS_TO_GENERATE):
                    print(f"{terminal_colors.WARNING}[WARNING]:max steps reached RUN_ID {run}{terminal_colors.ENDC}")
                    break

                # get program counter (to hex)
                instr_data = instr[0]
                step_attr = instr.attrib
                pc_hex = step_attr.get('pc')
                pc = int(pc_hex, 16)

                # to remove duplicate trace data
                # jan: TODO maybe move this into the analysis section
                if (not reached_start):  # dont create any objects if runs is identical to parent
                    # if(pc==branch_start or branch_start==-1 or (pc in potential_child_branches[parent_id])):
                    current_step = int(step_attr.get('step'))
                    # print(f"checking for run {run-1} and parent {parent_id} with pc({pc})==branch_pc{branch_pc} and step [{current_step}/{branch_step}]")
                    # jan: TODO probably need to log previous pc instead of branch pc after jumping
                    if (current_step >= branch_step):
                        if (pc == branch_pc or branch_pc <= 0):
                            reached_start = True
                            print(
                                f"-- Reached start of run {terminal_colors.OKBLUE}(removed duplicate trace data up to step {current_step}){terminal_colors.ENDC}")
                            # set parent for current run
                            current_run.start_step = start_step
                            current_run.start_pc = start
                        else:
                            reached_start = True
                            print(
                                f"-- {terminal_colors.OKBLUE}[INFO] Exceeded start step of run (starting from start step {current_step}){terminal_colors.ENDC}")
                            print(f"pc: {hex(pc)}, branch_pc:{hex(branch_pc)}")
                            current_run.start_step = start_step
                            current_run.start_pc = start
                    else:
                        # print(f"skipped pc {hex(pc)} (not {hex(branch_start)})")
                        if (instr_data.tag == "jump"):
                            link_reg = instr_data.attrib.get("link")
                            link_address = int(instr_data.attrib.get("link-address"), 16)
                            update_callstack(call_stack, link_reg, link_address)
                        continue  # CONTINUE with next loop

                # check if a block was already created for this pc, run and depth
                current_instruction = None
                object_already_exists = False
                for instruction in current_run.instruction_list:
                    if (instruction.depth == len(call_stack) and instruction.pc == pc):
                        object_already_exists = True
                        current_instruction = instruction
                        # print(f"instruction {instruction} already exists")
                        break

                # check if any input register or the result is symbolic for any instruction in this run
                reg_rs1, reg_rs1_symbolic = get_instr_reg(instr_data, "rs1")
                reg_rs2, reg_rs2_symbolic = get_instr_reg(instr_data, "rs2")
                imm = get_imm(instr_data, "imm")
                reg_rd, reg_rd_symbolic = get_instr_reg(instr_data, "rd")
                rd_was_symbolic = False

                # determine the symbolic behaviour
                symbolic_beh = determine_symbolic_behavior(instr_data, reg_rs1, reg_rs2, reg_rd,
                                                           reg_rs1_symbolic, reg_rs2_symbolic, reg_rd_symbolic,
                                                           rd_was_symbolic)

                if (not object_already_exists):
                    # determine e.g. type of instruction (Store, Arith, Load etc.)
                    current_instruction = process_instruction(run, instr, pc, call_stack, reg_rs1, reg_rs2, reg_rd, imm)
                if (current_instruction == None):
                    print("ERROR: instruction block wasn't created")  # should be unreachable if all are implemented

                # get step as string and int (e.g. step="23")
                step_attr = instr.attrib
                step_str = step_attr.get('step')
                step = int(step_str)

                if ((step, symbolic_beh) in current_instruction.steps_active):
                    print("ERROR: encountered duplicate step")
                    print(f"step={step} pc={hex(pc)} op={current_instruction.opcode.name}")
                if (instr_data.tag == "branch"):  # branches have a condition that can change between loops
                    branch_edge = int(instr_data.attrib.get("cond"))
                    current_instruction.add_active_step(step, symbolic_beh, branch_edge)
                else:
                    current_instruction.add_active_step(step, symbolic_beh)
                if (not object_already_exists):
                    current_run.instruction_list.append(current_instruction)
            if (not reached_start):
                print(
                    f"{terminal_colors.FAIL}[ERROR] could not find start for child run [{run - 1}] and parent [{parent_id}]{terminal_colors.ENDC}")
            print("      -- Done --\n")
            # Done processing this run
            current_run.num_steps = step_counter
            run_list.append(current_run)
        # else: not a symex run entry
    return run_list


def compute_code_files_lines(path_dir_elf, ptrace):
    with open(path_dir_elf, 'rb') as input_file:
        elf_file = ELFFile(input_file)

        if not elf_file.has_dwarf_info():
            print('file has no DWARF info')
            return
        dwarfinfo = elf_file.get_dwarf_info()
        for run in ptrace:
            for instruction in run.instruction_list:
                instruction.code_file, instruction.code_line, instruction.code_file_index = decode_file_line(dwarfinfo,
                                                                                                             instruction.pc)
                if (instruction.code_file == None) or (instruction.code_line == None) or (
                        instruction.code_file_index == None):
                    instruction.code_file = ""
                    instruction.code_line = -1
                    instruction.code_file_index = -1
                print(f"test, codeFile und codeLine: {instruction.code_file}, {instruction.code_line}")

    return ptrace


def make_ptrace_unique(ptrace):
    
    # TODO: would be nicer to copy instructions instead of whole run maybe?!
    copyPtrace = copy.deepcopy(ptrace)
    unique_ptrace = []
    runIndex = 0
    for run in ptrace:
        unique_ptrace.append(Run(run.run_id, run.start, run.end, run.num_steps))
        unique_ptrace[-1].set_parent(run.parent_id, run.start_step, run.start_pc)
        """unique_ptrace[-1].instruction_list.append(
            Instruction(run.instruction_list[0].pc, run.instruction_list[0].run_id, run.instruction_list[0].opcode,
                        run.instruction_list[0].depth, run.instruction_list[0].type))
        unique_ptrace[-1].instruction_list[-1].code_line = run.instruction_list[0].code_line
        unique_ptrace[-1].instruction_list[-1].code_file = run.instruction_list[0].code_file
        unique_ptrace[-1].instruction_list[-1].code_file_index = run.instruction_list[0].code_file_index
        # unique_ptrace[-1].instruction_list[-1].steps_active = [];
        # set first element with its steps
        for step in run.instruction_list[0].steps_active:
            condition = "True"
            target = -1
            if run.instruction_list[0].type == Opcode_type.Branch:
                condition = run.instruction_list[0].condition
            if run.instruction_list[0].type == Opcode_type.Branch or run.instruction_list[0].type == Opcode_type.Jump:
                target = run.instruction_list[0].target
            myid = step[0]
            mybeh = step[1]
            print(f"myid: {myid}, mybeh: {mybeh}")
            uniqueStep = UniqueStep(myid, mybeh, run.instruction_list[0].depth, condition, target)
            unique_ptrace[-1].instruction_list[0].steps_active.append(uniqueStep) """
        instructionIndex = 0
        for instruction in run.instruction_list:
            index = next((i for i, item in enumerate(unique_ptrace[-1].instruction_list) if item.pc == instruction.pc),
                         -1)
            if index >= 0:
                for step in instruction.steps_active:
                    condition = "True"
                    target = -1
                    if instruction.type == Opcode_type.Branch:
                        condition = instruction.condition
                    if instruction.type == Opcode_type.Branch or instruction.type == Opcode_type.Jump:
                        target = run.instruction_list[instructionIndex].target
                    uniqueStep = UniqueStep(step[0], step[1], instruction.depth, condition, target)
                    unique_ptrace[-1].instruction_list[index].steps_active.append(uniqueStep)
            else:

                unique_ptrace[-1].instruction_list.append(copyPtrace[runIndex].instruction_list[instructionIndex])

                """

                unique_ptrace[-1].instruction_list.append(
                    Instruction(instruction.pc, instruction.run_id, instruction.opcode, instruction.depth,
                                instruction.type))

                unique_ptrace[-1].instruction_list[-1].code_line = instruction.code_line
                unique_ptrace[-1].instruction_list[-1].code_file = instruction.code_file
                unique_ptrace[-1].instruction_list[-1].code_file_index = instruction.code_file_index  """

                unique_ptrace[-1].instruction_list[-1].steps_active = []
                for step in instruction.steps_active:
                    condition = "True"
                    target = -1
                    if instruction.type == Opcode_type.Branch:
                        condition = instruction.condition
                    if instruction.type == Opcode_type.Branch or instruction.type == Opcode_type.Jump:
                        target = instruction.target
                    uniqueStep = UniqueStep(step[0], step[1], instruction.depth, condition, target)
                    unique_ptrace[-1].instruction_list[index].steps_active.append(uniqueStep)
            instructionIndex += 1
        runIndex += 1
    return unique_ptrace


def search_animation_step(run_ptrace, RunIndex, stepId):
    """ returns animationStep with given stepId """
    # I know this is very ugly and probably extremely slow -> not as slow as I thought...

    InstructionIndex = 0
    StepIndex = 0

    for instruction in run_ptrace.instruction_list:
        for step in instruction.steps_active:
            if step.step_id == stepId:
                return AnimationStep(stepId, RunIndex, InstructionIndex, StepIndex)
            StepIndex += 1
        InstructionIndex += 1
        StepIndex = 0
    return None


def generate_animation_trace(unique_ptrace):

    print(f"Now in generate animation trace")

    animation_trace = [] # consists fo lists (of runs) of AnimationSteps
    currentStepId = 0

    Runindex = 0
    for run in unique_ptrace:

        print(f"Now in generate animation trace for run {Runindex}")

        # apparently runs can have no instructions at all.
        if not run.instruction_list or not run.instruction_list[0].steps_active:
            continue

        currentStepId = run.instruction_list[0].steps_active[0].step_id
        RunAnimation = []
        while True:
            NewAnimationStep = search_animation_step(run, Runindex, currentStepId)
            if NewAnimationStep is not None:
                RunAnimation.append(NewAnimationStep)
                currentStepId += 1
            else :
                animation_trace.append(RunAnimation)
                break
        Runindex += 1
    return animation_trace



def process_trace_file(input_path, output_path, path_dir_elf):
    while (input_path == ""):
        print(f"{terminal_colors.FAIL}[ERROR] empty input path{terminal_colors.ENDC}")
        input_path = input("Specify new path to trace:")

    tree, root = read_xml(input_path)

    # split intput rtrace into name, path etc.
    path, file = os.path.split(input_path)  # input_path.split("/")[-1].split(".")[0]
    executable_name = file.split(".")[0]
    print(f"{terminal_colors.HEADER}[Start processing trace {executable_name}]{terminal_colors.ENDC}")


    # Perform basic analysis on the trace to be able to detect any large gaps in memory/code later and improve visualization
    analysis_results = analyse_trace(root)
    print(f"{terminal_colors.OKGREEN}[Trace analysis complete]{terminal_colors.ENDC}")
    # determine stuff like instruction types, symbolic behaviour, remove duplicates etc...
    ptrace = process_trace(root, analysis_results)
    print(f"{terminal_colors.OKGREEN}[Trace processed]{terminal_colors.ENDC}")

    # write for each instruction the line and fileName it comes from
    ptrace = compute_code_files_lines(path_dir_elf, ptrace)

    unique_ptrace = make_ptrace_unique(ptrace)


    # now we just write to xml and json file and maybe even check them

    # parse analysis and ptrace to json and write it to file
    ptrace_json = '{\n  "ptrace_version": "4.0",\n'



    # Unique ptrace to json:

    # parse analysis and ptrace to json and write it to file
    # ptrace_json += '{\n  "ptrace_version": "4.0",\n'

    ptrace_json += '  "runs": {\n'
    ptrace_json += f'    "name": "{executable_name}",\n'
    ptrace_json += '    "run": [\n'

    for run in unique_ptrace:
        ptrace_json += run.to_json_unique()
    # remove last comma
    ptrace_json = ptrace_json[:-2]
    ptrace_json += '\n'

    ptrace_json += '    ]\n'
    ptrace_json += '  },\n'


    # TODO: Testing atm
    AnimationTrace = generate_animation_trace(unique_ptrace)
    # print(AnimationTrace)

    RunIndex = 0
    ptrace_json += '  "trace_animation": {\n'
    ptrace_json += '    "run_animations": [\n'
    for RunAnimation in AnimationTrace:
        ptrace_json += '      {\n'
        ptrace_json += f'        "run_index": {RunIndex},\n' # will probably never use this...
        ptrace_json += '        "animation_steps": [\n'
        for AnimationStep in RunAnimation:
            ptrace_json += AnimationStep.to_json()
        ptrace_json = ptrace_json[:-2]
        ptrace_json += '\n'
        ptrace_json += '        ]\n'
        ptrace_json += '      },\n'
        RunIndex += 1
    ptrace_json = ptrace_json[:-2]
    ptrace_json += '\n'
    ptrace_json += '  ]\n'
    ptrace_json += '  },\n'

    # needed for blocksCalculation
    maxPc = analysis_results.max_pc



    # analysis to json




    ptrace_json += analysis_results.to_json()
    ptrace_json += "}"
    save_file_name = f"{output_path}/out_json.ptrace"
    if (os.path.isfile(save_file_name)):
        if (confirm(
                f"PTrace file [{save_file_name}] already exists. Overwrite? {terminal_colors.OKGREEN}[y/yes]{terminal_colors.ENDC} ")):
            save_file_text(ptrace_json, save_file_name, True)
        else:
            print(f"{terminal_colors.FAIL}PTrace was not saved{terminal_colors.ENDC}")
    else:
        save_file_text(ptrace_json, save_file_name, False)
        print(f"{terminal_colors.OKCYAN}Saved ptrace as [{save_file_name}]{terminal_colors.ENDC}")

    # parse analysis and ptrace to xml and write it to file
    ptrace_xml = '<?xml version="1.0" encoding="UTF-8"?>'
    ptrace_xml += "<data ptrace_version=\"4.0\">\n"
    # add the analysis data to the xml
    ptrace_xml += analysis_results.to_xml()
    ptrace_xml += "</data>\n"
    save_file_name = f"{output_path}/{executable_name}.ptrace"
    if (os.path.isfile(save_file_name)):
        if (confirm(
                f"PTrace file [{save_file_name}] already exists. Overwrite? {terminal_colors.OKGREEN}[y/yes]{terminal_colors.ENDC} ")):
            save_file_text(ptrace_xml, save_file_name, True)
        else:
            print(f"{terminal_colors.FAIL}PTrace was not saved{terminal_colors.ENDC}")
    else:
        save_file_text(ptrace_xml, save_file_name, False)
        print(f"{terminal_colors.OKCYAN}Saved ptrace as [{save_file_name}]{terminal_colors.ENDC}")
    return len(ptrace), maxPc
