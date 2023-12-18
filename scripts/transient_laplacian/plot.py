import matplotlib.pyplot as plt
import numpy as np
import os
import os.path

if __name__ == "__main__":
    output_dir = "output/"
    svg_dir = "plots/"
    config_file_name = "config"

    # script_dir = os.path.dirname(__file__)
    # output_dir = os.path.join(script_dir, output_dir_base)
    # svg_dir = os.path.join(script_dir, svg_dir_base)
    try:
        os.mkdir(svg_dir)
    except OSError:
        pass
    
    case_directories = os.listdir(output_dir)

    for j in range(len(case_directories)):
        case_dir = os.path.join(output_dir, case_directories[j])
        config_file = os.path.join(case_dir, config_file_name)
        with open(config_file) as file:
            variables = file.readline().split(" ")
            tStart = float(file.readline().split(" ")[1])
            tEnd = float(file.readline().split(" ")[1])
            dt = float(file.readline().split(" ")[1])

        DIM = 1
        variables = variables[DIM:]

        fig, ax = plt.subplots(len(variables), 1, figsize=(6, 12))
        if (len(variables) == 1): ax = [ax]

        fig.suptitle(case_directories[j])

        output_files = sorted([file for file in os.listdir(case_dir) if (file != config_file_name)])

        for timestep_no in range(len(output_files)):
            infile = os.path.join(case_dir, output_files[timestep_no])

            with open(infile) as file:
                line = file.readline()
                time = float(line.split(" ")[1])

            # read data
            data = np.genfromtxt(infile, skip_header=1)

            for var_num in range(len(variables)):
                ax[var_num].plot(data[:, 0], data[:, var_num+DIM], label=f"t = {time}")
                ax[var_num].set_title(variables[var_num])
                ax[var_num].legend()

        fig.tight_layout()
        fig.savefig(os.path.join(svg_dir, f"{case_directories[j]}.svg"))
