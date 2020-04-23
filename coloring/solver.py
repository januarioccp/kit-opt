#!/usr/bin/python
# -*- coding: utf-8 -*-

import minizinc
from minizinc import Instance, Model, Solver
from datetime import timedelta

def solve_it(input_data):
    # Modify this code to run your optimization algorithm

    # parse the input
    lines = input_data.split('\n')

    first_line = lines[0].split()
    node_count = int(first_line[0])
    edge_count = int(first_line[1])

    edges = []
    nodes = [[0 for i in range(node_count)] for j in range(node_count)]
    for i in range(1, edge_count + 1):
        line = lines[i]
        parts = line.split()
        edges.append((int(parts[0]), int(parts[1])))
        nodes[int(parts[0])][int(parts[1])] = nodes[int(parts[1])][int(parts[0])] = 1

    gurobi = Solver.lookup("gecode") #gecode, gurobi, chuffed

    model = minizinc.Model()

    model = Model()

    model.add_string(" include \"alldifferent.mzn\"; ")
    model.add_string("set of int: color = 0.."+str(node_count-1)+"; \n ")
    model.add_string("array[color] of var color: x; \n ")

    # print("set of int: color = 0.."+str(node_count-1)+"; \n ")
    # print("array[color] of var color: x; \n ")

    for i in range(edge_count):
        # print("constraint x["+str(edges[i][0])+"] != x["+str(edges[i][1])+"];")
        model.add_string("constraint x["+str(edges[i][0])+"] != x["+str(edges[i][1])+"];\n ")

    for i in range(node_count):
        click = []
        click.append(i)
        for j in range(i,node_count):
            if nodes[j][i] == 1:
                insert = True
                for k in range(len(click)):
                    if nodes[j][click[k]] == 0:
                        insert = False
                        break
                if insert:
                    click.append(j)
        if len(click) > 2:
            # print(click)
            model.add_string("constraint alldifferent([")
            for k in range(len(click)):
                model.add_string("x["+str(click[k])+"]")
                if k < len(click)-1:
                    model.add_string(",")
            model.add_string("]); \n ")

    # print("solve minimize max(i in x)(i); \n ")
    model.add_string("solve minimize max(i in x)(i); \n ")

    instance = Instance(gurobi, model)
    result = instance.solve(timeout=timedelta(minutes=290))
    solution = result["x"]

    value = 0
    for i in range(node_count):
        value = max(value,solution[i]+1)

    # build a trivial solution
    # every node has its own color
    

    # prepare the solution in the specified output format
    output_data = str(value) + ' ' + str(0) + '\n'
    output_data += ' '.join(map(str, solution))

    return output_data


import sys

if __name__ == '__main__':
    import sys
    if len(sys.argv) > 1:
        file_location = sys.argv[1].strip()
        with open(file_location, 'r') as input_data_file:
            input_data = input_data_file.read()
        print(solve_it(input_data))
    else:
        print('This test requires an input file.  Please select one from the data directory. (i.e. python solver.py ./data/gc_4_1)')

