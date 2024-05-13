import argparse
import numpy as np
import json


parser = argparse.ArgumentParser()
parser.add_argument('a', type=str, help='path to first matrix for multiplication')
parser.add_argument('b', type=str, help='path to second matrix for multiplication')
parser.add_argument('res', type=str, help="path to matrices multiplication result")
args = parser.parse_args()

with open(args.a, "r") as first_matrix_file:
    with open(args.b, 'r') as second_matrix_file:
        first_matrix = np.array(json.loads(first_matrix_file.read())["matrix"])
        second_matrix = np.array(json.loads(second_matrix_file.read())["matrix"])
        with open(args.res, "r") as res:
            if not np.array_equal(np.dot(first_matrix, second_matrix), np.array(json.loads(res.read())["matrix"])):
                print("Multiplying error")

