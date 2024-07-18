from enums.riscv_enum import Reg, Opcode_type, Symbolic_Beh, Opcode

STORE_OPCODES = [Opcode.SB, Opcode.SH, Opcode.SW, Opcode.SC_W, Opcode.SC_D, Opcode.FSW]


class Link_Data:
    """"from timeliness in rtrace"""

    def __init__(self, hash, pc, step):
        self.hash = hash
        self.pc = pc
        self.step = step


class Link_Node:
    """Binary tree node holding a hash and a weight"""

    def __init__(self, data, run_id, parent):
        self.left = None
        self.right = None
        self.parent = parent
        self.data = data
        self.run_id = run_id
        self.weight = 1

    def insert(self, p_data, run_id, weight):
        """inserts a new node or increase weight if that node already exists
        Returns:
            the inserted node or the identical existing node"""
        if self.left is not None:
            if p_data.hash == self.left.data.hash:  # has a left child and its identical
                self.left.weight += 1
                return self.left
            elif self.right is not None:
                if p_data.hash == self.right.data.hash:  # has a left and right child and right is identical
                    self.right.weight += 1
                    return self.right
                else:
                    print(f"[ERROR] Node with hash {self.data.hash} already has two children (new hash: {p_data.hash})")
            else:
                self.right = Link_Node(p_data, run_id, self)
                return self.right
        else:
            self.left = Link_Node(p_data, run_id, self)
            return self.left

    def discover_run_start(self, run_id):
        """Finds the first occurrence of a run"""
        if (self.run_id == run_id):
            return self.parent
        elif (self.left is not None):
            result = self.left.discover_run_start(run_id)
            if (result is not None):
                return result
            elif (self.right is not None):  # if left is None, right is always None
                result = self.right.discover_run_start(run_id)
                if (result is not None):
                    return result
        return None

    def PrintTree(self):
        str = "("
        if self.left:
            str += self.left.PrintTree()
        str += ")"
        if (self.data is not None):
            str += f"[{self.data.hash}]"
        else:
            str += "ROOT"
        str += "("
        if self.right:
            str += self.right.PrintTree()
        str += ")"
        return str

    def display(self):
        lines, *_ = self._display_aux()
        for line in lines:
            print(line)

    def _display_aux(self):
        """Returns list of strings, width, height, and horizontal coordinate of the root."""
        # No child.
        if self.right is None and self.left is None:
            line = f"[ROOT:{self.run_id}:{self.weight}]"
            if (self.data is not None):
                line = f"[{str(self.data.hash)[0:4]}:{self.run_id}:{self.weight}]"
            width = len(line) - 2
            height = 1
            middle = width // 2
            return [line], width, height, middle

        # Only left child.
        if self.right is None:
            lines, n, p, x = self.left._display_aux()
            s = f"[ROOT:{self.run_id}:{self.weight}]"
            if (self.data is not None):
                s = f"[{str(self.data.hash)[0:4]}:{self.run_id}:{self.weight}]"
            u = int((len(s) - 6))
            first_line = (x + 1) * ' ' + s
            second_line = (x + 1) * ' ' + '/' + (n - x - 1 + u) * ' '
            shifted_lines = [line + u * ' ' for line in lines]
            return [first_line, second_line] + shifted_lines, n + u, p + 2, n + u // 2

        # Only right child.
        if self.left is None:
            lines, n, p, x = self.right._display_aux()
            s = f"[ROOT:{self.run_id}:{self.weight}]"
            if (self.data is not None):
                s = f"[{str(self.data.hash)[0:4]}:{self.run_id}:{self.weight}]"
            u = int((len(s) - 6))
            first_line = s + x * '_' + (n - x) * ' '
            second_line = (u + x) * ' ' + '\\' + (n - x - 1) * ' '
            shifted_lines = [u * ' ' + line for line in lines]
            return [first_line, second_line] + shifted_lines, n + u, p + 2, u // 2

        # Two children.
        left, n, p, x = self.left._display_aux()
        right, m, q, y = self.right._display_aux()
        s = f"[ROOT:{self.run_id}:{self.weight}]"
        if (self.data is not None):
            s = f"[{str(self.data.hash)[0:4]}:{self.run_id}:{self.weight}]"
        u = len(s) - 2
        first_line = (x + 1) * ' ' + (n - x - 1) * '_' + s + y * '_' + (m - y) * ' '
        second_line = x * ' ' + '/' + (n - x - 1 + u + y) * ' ' + '\\' + (m - y - 1) * ' '
        if p < q:
            left += [n * ' '] * (q - p)
        elif q < p:
            right += [m * ' '] * (p - q)
        zipped_lines = zip(left, right)
        lines = [first_line, second_line] + [a + u * ' ' + b for a, b in zipped_lines]
        return lines, n + m + u, max(p, q) + 2, n + u // 2


# analysis data for analysis results with functions for toXML etc.
class Analysis_Data:
    global_start = 0
    min_pc = 0xFFFFFF
    max_pc = 0

    num_runs = 0
    timeline_forks = [0]
    run_start = []  # [run]
    potential_child_branches = []  # [run][potential child branches] Link_Data
    memory_list = []
    memory_list_per_run = []

    discovered_run_links = []  # [run_parent][(child,start_pc,start_step)]

    def __init__(self, global_start, min_pc, max_pc, num_runs, timeline_forks, run_start,
                 potential_child_branches, memory_list, memory_list_per_run):
        self.global_start = global_start
        self.min_pc = min_pc
        self.max_pc = max_pc

        self.num_runs = num_runs
        self.timeline_forks = timeline_forks
        self.run_start = run_start
        self.potential_child_branches = potential_child_branches
        self.memory_list = memory_list
        self.memory_list_per_run = memory_list_per_run

    # writes the whole analysis data into a jsonString
    def to_json(self):
        json_string = '  "analysis": {\n'
        json_string += f'    "global_start": {self.global_start},\n'
        json_string += f'    "global_start_hex": "{hex(self.global_start)}",\n'
        json_string += f'    "min_pc": {self.min_pc},\n'
        json_string += f'    "min_pc_hex": "{hex(self.min_pc)}",\n'
        json_string += f'    "max_pc": {self.max_pc},\n'
        json_string += f'    "max_pc_hex": "{hex(self.max_pc)}",\n'
        json_string += f'    "num_runs": {self.num_runs},\n'
        json_string += f'    "timeline_forks": "{self.timeline_forks}",\n'
        json_string += f'    "potential_children": [\n'

        for run in range(self.num_runs):
            json_string += "      {\n"
            _run_parent = -1
            _run_start_pc = -1
            _run_start_step = 0
            if (len(self.run_start) > run):
                _run_parent = self.run_start[run][0]
                _run_start_pc = self.run_start[run][1]
                _run_start_step = self.run_start[run][2]
            else:
                print(f"[ERROR] missing run start data for run {run + 1}")
            json_string += f'        "run_id": {run},\n'
            json_string += f'        "run_start_pc": "{hex(_run_start_pc)}",\n'
            json_string += f'        "run_start_step": {_run_start_step},\n'
            json_string += f'        "run_parent": {_run_parent},\n'
            json_string += f'        "potential_child": [\n'
            for link_data in self.potential_child_branches[run]:
                json_string += "          {\n"
                json_string += f'            "hash": {link_data.hash},\n'
                json_string += f'            "pc": "{hex(link_data.pc)}",\n'
                json_string += f'            "step": {link_data.step}\n'
                json_string += "          },\n"
            json_string = json_string[:-2]
            json_string += "\n"
            json_string += f'        ]\n'
            json_string += "      },\n"
        json_string = json_string[:-2]
        json_string += "\n"
        json_string += f'    ]\n'
        json_string += '  }\n'
        return json_string

    # writes the whole analysis data into a xmlString
    def to_xml(self):
        xml_string = f'<Analysis global_start="{hex(self.global_start)}" min_pc="{hex(self.min_pc)}" '
        xml_string += f'max_pc="{hex(self.max_pc)}" num_runs="{self.num_runs}" timeline_forks="{self.timeline_forks}">\n '
        xml_string += '<potential_children>'

        for run in range(self.num_runs):
            _run_parent = -1
            _run_start_pc = -1
            _run_start_step = 0
            if (len(self.run_start) > run):
                _run_parent = self.run_start[run][0]
                _run_start_pc = self.run_start[run][1]
                _run_start_step = self.run_start[run][2]
            else:
                print(f"[ERROR] missing run start data for run {run + 1}")
            xml_string += f'<run id="{run}" run_start_pc="{hex(_run_start_pc)}" run_start_step="{_run_start_step}" run_parent="{_run_parent}">\n'
            for link_data in self.potential_child_branches[run]:
                xml_string += f'     <potential_child hash="{link_data.hash}" pc="{hex(link_data.pc)}" step="{link_data.step}"></potential_child>\n'
            xml_string += f'</run>\n'
        xml_string += '</potential_children>\n'

        xml_string += '<discovered_links>'
        dc_run_id = -1
        for run in self.discovered_run_links:
            dc_run_id += 1
            xml_string += f'<run id="{dc_run_id}" >\n'
            for child, start_pc, start_step in run:
                xml_string += f'     <child id="{child}" pc="{start_pc}" step="{start_step}" ></child>\n'
            xml_string += '</run>\n'
        xml_string += '</discovered_links>'

        xml_string += '<memory>\n'
        i = 0
        for address in self.memory_list:
            i += 1
            xml_string += f'  <address value="{address}"></address>'
            if (i % 4 == 0):
                xml_string += "\n"
        xml_string += '</memory>\n'
        xml_string += '<memory_per_run>\n'
        for run_id, c_run in enumerate(self.memory_list_per_run):
            xml_string += f'<run id="{run_id}">\n'
            for i, address in enumerate(c_run):
                i += 1
                xml_string += f'  <address value="{address}"></address>'
                if (i % 4 == 0):
                    xml_string += "\n"
            xml_string += '</run>\n'
        xml_string += '</memory_per_run>\n'

        xml_string += f'</Analysis>\n'

        return xml_string


class Run:
    """a single path explored by the SymEx engine
    """
    run_id = -1
    start = -1
    end = -1
    num_steps = 0

    parent_id = -1
    start_step = 0
    start_pc = -1
    # children = [] #(run_id,step)

    """list of instructions in this run """
    instruction_list = []  # [id]=[Instruction]

    def __init__(self, run_id, start, end=-1, num_steps=0):
        self.run_id = run_id
        self.start = start
        self.end = end
        self.num_steps = num_steps

        # self.children = []
        self.instruction_list = []

    def set_parent(self, parent_id, start_step, start_pc):
        self.parent_id = parent_id
        self.start_step = start_step
        self.start_pc = start_pc


    # convert this run to a unique json string
    def to_json_unique(self):
        json_string = '      {\n'
        json_string += f'        "run_id": {str(self.run_id)},\n'
        json_string += f'        "start": {str(self.start)},\n'
        json_string += f'        "start_hex": "{hex(self.start)}",\n'
        json_string += f'        "end": {str(self.end)},\n'
        json_string += f'        "end_hex": "{hex(self.end)}",\n'
        json_string += f'        "steps": {str(self.num_steps)},\n'
        json_string += f'        "parent": {str(self.parent_id)},\n'
        json_string += f'        "start_step": {str(self.start_step)},\n'
        json_string += f'        "start_pc": {str(self.start_pc)},\n'
        json_string += f'        "start_pc_hex": "{hex(self.start_pc)}",\n'

        json_string += f'        "instruction": [\n'

        for instruction in self.instruction_list:
            json_string += '          {\n'
            json_string += instruction.to_json_p1_unique() + instruction.to_json_p2_unique() + instruction.to_json_p3_unique()
            json_string += '          },\n'
        json_string = json_string[:-2]
        json_string += '\n'

        json_string += f'        ]\n'
        json_string += '      },\n'

        return json_string

    def to_json(self):
        # convert this run to a json string
        # TODO: maybe rename a few for more clarity
        json_string = '      {\n'
        json_string += f'        "run_id": {str(self.run_id)},\n'
        json_string += f'        "start": {str(self.start)},\n'
        json_string += f'        "end": {str(self.end)},\n'
        json_string += f'        "steps": {str(self.num_steps)},\n'
        json_string += f'        "parent": {str(self.parent_id)},\n'
        json_string += f'        "start_step": {str(self.start_step)},\n'
        json_string += f'        "start_pc": {str(self.start_pc)},\n'

        json_string += f'        "instruction": [\n'

        for instruction in self.instruction_list:
            json_string += '          {\n'
            json_string += instruction.to_json_p1() + instruction.to_json_p2() + instruction.to_json_p3()
            json_string += '          },\n'
        json_string = json_string[:-2]
        json_string += '\n'

        json_string += f'        ]\n'
        json_string += '      },\n'

        return json_string

    # convert this run to a xml string
    def to_xml(self):
        xml_string = f'<run id="{self.run_id}" start="{self.start}" '
        xml_string += f'end="{self.end}" steps="{self.num_steps}" parent="{self.parent_id}" '
        xml_string += f'start_step="{self.start_step}" start_pc="{self.start_pc}" >'

        for instruction in self.instruction_list:
            xml_string += instruction.to_xml_p1() + instruction.to_xml_p2() + instruction.to_xml_p3()

        xml_string += "</run>"
        return xml_string


class UniqueStep:
    step_id = -1
    symbolic_beh = Symbolic_Beh.none
    depth = -1

    # will only be used for branches / jumps
    condition = "True"
    target = -1

    def __init__(self, step_id, symbolic_beh, depth, condition, target):
        self.step_id = step_id
        self.symbolic_beh = symbolic_beh
        self.depth = depth
        self.condition = condition
        self.target = target



class Instruction:
    """ Basic Instruction
        Currently used by ECALL
    """
    pc = -1
    run_id = -1
    opcode = Opcode.ADD
    code_line = -1
    code_file = ""
    code_file_index = -1

    depth = 0

    steps_active = []  # (step, Symbolic_Beh)
    type = Opcode_type.none

    def __init__(self, pc, run_id, opcode, depth, type=Opcode_type.none):
        self.pc = pc
        self.run_id = run_id
        self.opcode = opcode
        self.depth = depth
        self.steps_active = []
        self.type = type

    def add_active_step(self, step, mode):
        if ((step, mode) in self.steps_active):
            print("ERROR step was already added")
        self.steps_active.append((step, mode))

    def to_json_p1_unique(self):
        json_string = f'            "pc": {self.pc},\n'
        json_string += f'            "pc_hex": "{hex(self.pc)}",\n'
        json_string += f'            "run_id": {self.run_id},\n'
        json_string += f'            "code_file_name": "{self.code_file}",\n'  # can be deleted (just for checking correctness right now)
        json_string += f'            "code_line": {self.code_line},\n'
        json_string += f'            "code_file_id": {self.code_file_index},\n'
        json_string += f'            "opcode": "{self.opcode.name}",\n'
        json_string += f'            "type": "{self.type.name}",\n'
        return json_string


    def to_json_p1(self):
        json_string = f'            "pc": {self.pc},\n'
        json_string += f'            "pc_hex": "{hex(self.pc)}",\n'
        json_string += f'            "run_id": {self.run_id},\n'
        json_string += f'            "code_file_name": "{self.code_file}",\n'  # can be deleted (just for checking correctness right now)
        json_string += f'            "code_line": {self.code_line},\n'
        json_string += f'            "code_file_id": {self.code_file_index},\n'
        json_string += f'            "opcode": "{self.opcode.name}",\n'
        json_string += f'            "type": "{self.type.name}",\n'
        json_string += f'            "depth": {self.depth},\n'
        return json_string

    def to_json_p2_unique(self):
        """ Instruction specific json conversion function
            should be overridden to include instruction specific attributes """
        # TODO: override in specific instructions
        json_string = ''
        return json_string

    def to_json_p2(self):
        """ Instruction specific json conversion function
            should be overridden to include instruction specific attributes """
        # TODO: override in specific instructions
        json_string = ''
        return json_string

    def to_json_p3_unique(self):
        """ Base xml conversion function. Should not be overridden.
            converts info for all active steps """
        json_string = '            "step": [\n'
        for step in self.steps_active:
            json_string += '              {\n'
            json_string += f'                "id": {step.step_id},\n'
            json_string += f'                "beh": "{Symbolic_Beh(step.symbolic_beh).name}",\n'
            json_string += f'                "depth": {step.depth},\n'
            # json_string += f'                "condition": {step.condition},\n'
            json_string += f'                "target": {step.target}\n'
            json_string += '              },\n'
        json_string = json_string[:-2]
        json_string += '\n'
        json_string += '            ]\n'
        return json_string


    def to_json_p3(self):
        """ Base json conversion function. Should not be overridden.
            converts info for all active steps """
        json_string = '            "step": [\n'
        for step in self.steps_active:
            json_string += '              {\n'
            json_string += f'                "id": {step[0]},\n'
            json_string += f'                "beh": "{Symbolic_Beh(step[1]).name}"\n'
            json_string += '              },\n'
        json_string = json_string[:-2]
        json_string += '\n'
        json_string += '            ]\n'
        return json_string

    def to_xml_p1(self):
        """ Base xml conversion function
        """
        xml_string = f'<instruction pc="{hex(self.pc)}" '
        xml_string += f'run_id="{self.run_id}" '
        xml_string += f'opcode="{self.opcode.name}" '
        xml_string += f'type="{self.type.name}" '
        xml_string += f'depth="{self.depth}" '

        return xml_string

    def to_xml_p2(self):
        """ Instruction specific xml conversion function
            should be overridden to include instruction specific attributes
        """
        xml_string = ' >'
        return xml_string

    def to_xml_p3(self):
        """ Base xml conversion function
            Should not be overridden
            converts info for all active steps
        """
        xml_string = ""
        if (len(self.steps_active) > 0):
            xml_string += "\n"
        for step in self.steps_active:
            xml_string += '<step '
            xml_string += f'id="{step[0]}" beh="{Symbolic_Beh(step[1]).name}" >'
            xml_string += '</step>\n'
        xml_string += '</instruction>'
        return xml_string


# will be used as an animation step in another value in json ptrace
class AnimationStep:

    StepId = -1
    RunIndex = -1
    InstructionIndex = -1
    StepIndex = -1

    def __init__(self, stepId, RunIndex, InstructionIndex, StepIndex):
        self.StepId = stepId
        self.RunIndex = RunIndex
        self.InstructionIndex = InstructionIndex
        self.StepIndex = StepIndex

    def to_json(self):
        json_string = '        {\n'
        json_string += f'          "step_id": {self.StepId},\n'
        json_string += f'          "run_index": {self.RunIndex},\n'
        json_string += f'          "instruction_index": {self.InstructionIndex},\n'
        json_string += f'          "step_index": {self.StepIndex}\n'
        json_string += '        },\n'
        return json_string




class Arith_Instruction(Instruction):
    """ Basic Instruction with no direct influence on controlflow
    """

    reg_rs1 = Reg.none
    reg_rs2 = Reg.none
    reg_rd = Reg.none

    imm1 = None
    imm2 = None

    def __init__(self, pc, run_id, opcode, reg_rs1=Reg.none, reg_rs2=Reg.none, reg_rd=Reg.none,
                 imm1=None, imm2=None, depth=0):
        super().__init__(pc, run_id, opcode, depth, type=Opcode_type.Arith)

        self.reg_rs1 = reg_rs1
        self.reg_rs2 = reg_rs2
        self.reg_rd = reg_rd

        self.imm1 = imm1
        self.imm2 = imm2

    def to_json_p2_unique(self):
        """ Instruction specific json conversion function """
        json_string = f'            "rs1": "{self.reg_rs1.name}",\n'
        json_string += f'            "rs2": "{self.reg_rs2.name}",\n'
        json_string += f'            "rd": "{self.reg_rd.name}",\n'
        json_string += f'            "imm1": "{self.imm1}",\n'
        json_string += f'            "imm2": "{self.imm2}",\n'
        return json_string

    def to_json_p2(self):
        """ Instruction specific json conversion function """
        json_string = f'            "rs1": "{self.reg_rs1.name}",\n'
        json_string += f'            "rs2": "{self.reg_rs2.name}",\n'
        json_string += f'            "rd": "{self.reg_rd.name}",\n'
        json_string += f'            "imm1": "{self.imm1}",\n'
        json_string += f'            "imm2": "{self.imm2}",\n'
        return json_string

    def to_xml_p2(self):
        """ Instruction specific xml conversion function
        """
        xml_string = f'rs1="{self.reg_rs1.name}" '
        xml_string += f'rs2="{self.reg_rs2.name}" '
        xml_string += f'rd="{self.reg_rd.name}" '

        xml_string += f'imm1="{self.imm1}" '
        xml_string += f'imm2="{self.imm2}" '

        xml_string += ' >'
        return xml_string


class Jump(Instruction):
    target = -1
    link_reg = Reg.none
    link_address = -1

    def __init__(self, pc, run_id, opcode, target, link_reg, link_address, depth=0):
        super().__init__(pc, run_id, opcode, depth, type=Opcode_type.Jump)

        self.target = target
        self.link_reg = link_reg
        self.link_address = link_address

    def to_json_p2_unique(self):
        """ Instruction specific json conversion function """
        json_string = f'            "link": "{self.link_reg.name}",\n'
        json_string += f'            "link_address": {self.link_address},\n'
        json_string += f'            "link_address_hex": "{hex(self.link_address)}",\n'
        return json_string

    def to_json_p2(self):
        """ Instruction specific json conversion function """
        json_string = f'            "target": "{self.target}",\n'
        json_string += f'            "link": "{self.link_reg.name}",\n'
        json_string += f'            "link_address": {self.link_address},\n'
        return json_string

    def to_xml_p2(self):
        """ Instruction specific xml conversion function
        """
        xml_string = f'target="{self.target}" '
        xml_string += f'link="{self.link_reg.name}" '
        xml_string += f'link_address="{self.link_address}" '

        xml_string += ' >'
        return xml_string


class Branch(Instruction):
    target = -1
    reg_rs1 = Reg.none
    reg_rs2 = Reg.none

    condition = False

    def __init__(self, pc, run_id, opcode, target, reg_rs1, reg_rs2, condition, depth=0):
        super().__init__(pc, run_id, opcode, depth, type=Opcode_type.Branch)

        self.target = target
        self.reg_rs1 = reg_rs1
        self.reg_rs2 = reg_rs2
        self.condition = condition

    def add_active_step(self, step, mode, edge):
        if ((step, mode, edge) in self.steps_active):
            print("[ERROR] step was already added")
        self.steps_active.append((step, mode, edge))

    def to_json_p2_unique(self):
        """ Instruction specific json conversion function """
        json_string = f'            "rs1": "{self.reg_rs1.name}",\n'
        json_string += f'            "rs2": "{self.reg_rs2.name}",\n'
        return json_string

    def to_json_p2(self):
        """ Instruction specific json conversion function """
        json_string = f'            "target": "{self.target}",\n'
        json_string += f'            "rs1": "{self.reg_rs1.name}",\n'
        json_string += f'            "rs2": "{self.reg_rs2.name}",\n'
        json_string += f'            "condition": "{self.condition}",\n'
        return json_string


    def to_json_p3_unique(self):
        """ Base xml conversion function. Should not be overridden.
            converts info for all active steps """
        json_string = '            "step": [\n'
        for step in self.steps_active:
            json_string += '              {\n'
            json_string += f'                "id": {step.step_id},\n'
            json_string += f'                "beh": "{Symbolic_Beh(step.symbolic_beh).name}",\n'
            json_string += f'                "depth": {step.depth},\n'
            json_string += f'                "condition": "{step.condition}",\n'
            json_string += f'                "target": {step.target}\n'
            json_string += '              },\n'
        json_string = json_string[:-2]
        json_string += '\n'
        json_string += '            ]\n'
        return json_string


    def to_json_p3(self):
        """ Base xml conversion function gets overridden by subclasses (extra edge member) """
        json_string = '            "step": [\n'
        for step in self.steps_active:
            json_string += '              {\n'
            json_string += f'                "id": {step[0]},\n'
            json_string += f'                "beh": "{Symbolic_Beh(step[1]).name}",\n'
            json_string += f'                "edge": {step[2]}\n'
            json_string += '              },\n'
        json_string = json_string[:-2]
        json_string += '\n'
        json_string += '            ]\n'
        return json_string

    def to_xml_p2(self):
        """ Instruction specific xml conversion function """
        xml_string = f'target="{self.target}" '
        xml_string += f'rs1="{self.reg_rs1.name}" '
        xml_string += f'rs2="{self.reg_rs2.name}" '
        xml_string += f'condition="{self.condition}" '

        xml_string += ' >'
        return xml_string

    def to_xml_p3(self):
        xml_string = ""
        if (len(self.steps_active) > 0):
            xml_string += "\n"
        for step in self.steps_active:
            xml_string += '<step '
            xml_string += f'id="{step[0]}" beh="{Symbolic_Beh(step[1]).name}" edge="{step[2]}" >'
            xml_string += '</step>\n'
        xml_string += '</instruction>'
        return xml_string


class LoadStore(Instruction):
    target = -1
    reg_rs1 = Reg.none
    imm1 = None
    reg_rs2d = Reg.none

    def __init__(self, pc, run_id, opcode, target, reg_rs1, imm1, reg_rs2d, depth=0):
        m_type = Opcode_type.Load
        if (opcode in STORE_OPCODES):
            m_type = Opcode_type.Store

        super().__init__(pc, run_id, opcode, depth, type=m_type)

        self.target = target
        self.reg_rs1 = reg_rs1
        self.imm1 = imm1
        self.reg_rs2d = reg_rs2d


    def to_json_p2_unique(self):
        """ Instruction specific json conversion function """
        json_string = f'            "rs1": "{self.reg_rs1.name}",\n'
        json_string += f'            "imm2": "{self.imm1}",\n'
        json_string += f'            "rs2d": "{self.reg_rs2d.name}",\n'
        return json_string


    def to_json_p2(self):
        """ Instruction specific json conversion function """
        json_string = f'            "target": "{hex(self.target)}",\n'
        json_string += f'            "rs1": "{self.reg_rs1.name}",\n'
        json_string += f'            "imm2": "{self.imm1}",\n'
        json_string += f'            "rs2d": "{self.reg_rs2d.name}",\n'
        return json_string

    def to_xml_p2(self):
        """ Instruction specific xml conversion function """
        # kw: TODO: why here hex all of a sudden?
        # my guess: we might need to compute further on jump/branch and not on this one...
        xml_string = f'target="{hex(self.target)}" '
        xml_string += f'rs1="{self.reg_rs1.name}" '
        xml_string += f'imm1="{self.imm1}" '
        xml_string += f'rs2d="{self.reg_rs2d.name}" '

        xml_string += ' >'
        return xml_string


class CSR(Instruction):
    flags = -1

    def __init__(self, pc, run_id, opcode, flags, depth=0):
        super().__init__(pc, run_id, opcode, depth, type=Opcode_type.CSR)

        self.flags = flags


