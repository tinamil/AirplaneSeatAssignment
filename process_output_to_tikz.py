from pathlib import Path
import re
import sys

type_vals = ['logs_middle_seat/vertex_packing/non_coughing']
type_vals = ['c:/users/john/even.txt']
for type_val in type_vals:
    #directory = type_val
    #output = 'tikz/' + type_val
    pattern = re.compile(r'[xo]+')
    radius = 0.125

    # for path in Path(directory).rglob('*.txt'):
    # print(path.absolute())
    path = type_val
    with open(path) as f:
        #Path(output + str(path.parent)).mkdir(parents=True, exist_ok=True)
        # with open(output + str(path), 'w') as out:
        out = sys.stdout
        out.write('\\begin{tikzpicture}\n')
        y_val = 0.0
        begin_graph = False
        for line in f:
            if 'Optimal value:' in line:
                begin_graph = True
                continue
            if begin_graph:
                x_val = 0.0
                for char in line:
                    if char is 'x':
                        out.write(
                            f'\\draw[fill=black] ({x_val:.3f},{y_val:.3f}) circle ({radius:.3f});\n')
                    if char is 'o':
                        out.write(
                            f'\\draw[fill=white] ({x_val - radius:.3f},{y_val - radius:.3f}) rectangle ({x_val + radius:.3f},{y_val + radius:.3f});\n')
                    x_val += 2.5 * radius
                y_val -= 3 * radius
        out.write('\\end{tikzpicture}\n')
