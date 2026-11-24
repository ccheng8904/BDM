import os
import sys

# Process arguments
out_file = sys.argv[1]
out_str = sys.argv[2]
out_dir = os.path.dirname(out_file)

# Create folder if not exists
if not os.path.exists(out_dir):
    os.makedirs(out_dir)

# Print to file (replace old one)
with open(out_file, 'w') as f:
   f.write(out_str)
