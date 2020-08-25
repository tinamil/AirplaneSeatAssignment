from pathlib import Path
import re

type_pattern = re.compile(r"(.+)\.graph\d+\.txt")
size_pattern = re.compile(r".+\.graph(\d+)\.txt", re.IGNORECASE)
optimality_pattern = re.compile(r"Solution status: (\w+)", re.IGNORECASE)
time_pattern = re.compile(r"Total \(root\+branch&cut\) =\s*([\d\.]+) sec.+", re.IGNORECASE)
risk_pattern = re.compile(r"Optimal value:\s+([\d\.]+)", re.IGNORECASE)

directory = "logs/graph_data/noncoughing/"
output = 'processed_data/'
pattern = re.compile(r'[xo]+')
radius = 0.125

loaded = set()
for path in sorted(Path(directory).rglob('*.txt'), key=lambda x: float(size_pattern.search(str(x)).group(1))):
    print(path.absolute())
    with open(path) as input_file:
        whole_file = input_file.read()
        if 'Solution status: Infeasible' in whole_file:
            continue
        output_file = output + str(path.parent) + "/" + type_pattern.search(path.name).group(1) + ".csv"
        Path(output + str(path.parent)).mkdir(parents=True, exist_ok=True)
        first_load = output_file not in loaded
        if first_load:
            loaded.add(output_file)
        file_mod = 'w' if first_load else 'a'
        with open(output_file, file_mod) as out:
            if first_load:
                out.write("N,Optimal/Feasible,Time,Risk,Marginal\n")
            n = size_pattern.search(path.name).group(1)
            optimal = optimality_pattern.search(whole_file).group(1)
            time = time_pattern.search(whole_file).group(1)
            risk = risk_pattern.search(whole_file).group(1)
            if first_load:
                out.write(n + "," + optimal + "," + time + "," + risk + "\n")
            else:
                out.write(n + "," + optimal + "," + time + "," + risk + "," + f"=D{int(n)+1} - D{n}\n")
