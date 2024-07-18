import xml.etree.ElementTree as ET


class terminal_colors:
    """ just some colors for terminal """
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'


def open_file(path):
    """ opens a file and returns its content as AnyStr """
    input_path = path
    try:
        with open(input_path, "rb") as input_file:
            data = input_file.read()
    except OSError as exception:
        print(f"Couldn't load {input_path}: {exception}")
        return
    return data


def save_file_text(data, path, overwrite=False):
    """ tries to write file """
    output_path = path
    mode = "xt"
    if (overwrite):
        mode = "wt"
    try:
        with open(output_path, mode) as output_file:
            output_file.write(data)
    except OSError as exception:
        print(f"Couldn't load {output_path}: {exception}")
        return
    return data


def read_xml(path):
    """ parse xml document into elementTree and return tree with root """
    tree = ET.parse(path)
    root = tree.getroot()
    return (tree, root)


def confirm(message):
    """ to confirm an overwrite if a file already exists """
    answer = input(message)
    if (answer.lower() in ["y", "yes"]):
        return True
    else:
        return False
