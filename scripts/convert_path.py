import sys
import json

if __name__ == "__main__":
  if len(sys.argv) < 2:
    print "Give a file yo"
    exit(0)
  for file_name in sys.argv[1:]:
    infile = open(file_name, 'r')
    to_json = []
    for line in infile:
      tokens = line.split()
      to_json.append((int(tokens[0]), float(tokens[1]), float(tokens[2])))
    infile.close()
    outfile = open(file_name, 'w')
    outfile.write(json.dumps(to_json))
    outfile.close()