"""
Copyright (c) 2020 BSH Hausgeraete GmbH,
Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de

All rights reserved. This program and the accompanying materials
are protected by international copyright laws.
Please contact copyright holder for licensing information.

Description      : Creates a SHA256 of a given file.
"""

import hashlib
from argparse import ArgumentParser

def get_digest(file_path):
    h = hashlib.sha256()

    with open(file_path, 'rb') as file:
        while True:
            # Reading is buffered, so we can read smaller chunks.
            chunk = file.read(h.block_size)
            if not chunk:
                break
            h.update(chunk)

    return h.hexdigest()


def main():

    # argument parsing
    argP = ArgumentParser(description='Creates a SHA256 of a given file.')
    
    argP.add_argument("-f",  dest="hash_file", required=True, help="file to create hash for")
    args = argP.parse_args()

    print(get_digest(args.hash_file))


    # ---------- end of main ----------

if __name__ == '__main__':
    main()

    
    
    