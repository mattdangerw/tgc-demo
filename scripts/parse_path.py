import sys
import inspect
from svgfig import *

output_paths = []

class OutputPath:
  def __init__(self):
    self.path = []
  # min_x = float("inf")
  # max_x = float("-inf")
  # min_y = float("inf")
  # max_y = float("-inf")
  def add(self, type, x, y):
    # y = -y
    self.path.append((type, x, y))
    # if x < self.min_x:
    #   self.min_x = x
    # if x > self.max_x:
    #   self.max_x = x
    # if y < self.min_y:
    #   self.min_y = y
    # if y > self.max_y:
    #   self.max_y = y
  def write(self, name):
    out = open(name, "w")
    scale = 1.0 / 1000
    for (type, x, y) in self.path:
      x *= scale
      y = 1 - y * scale
      out.write(" ".join((str(type), str(x), str(y))))
      out.write("\n")
    out.close()

def parsePath(path):
  output_path = OutputPath()
  figPath = pathtoPath(path)
  for command in figPath.d:
    command_type = command[0]
    if command_type == "M":
      output_path.add(1, command[1], command[2])
    elif command_type == "C":
      output_path.add(3, command[1], command[2])
      output_path.add(3, command[4], command[5])
      output_path.add(1, command[7], command[8])
    elif command_type == "Q":
      output_path.add(2, command[1], command[2])
      output_path.add(1, command[4], command[5])
    elif command_type == "m" or command_type == "c" or command_type == "q":
      print "Relative coordinates not supported. Fix in Inkscape."
  output_paths.append(output_path)


def findPaths(elem):
  if elem.__class__ == SVG:
    if elem.t == "path":
      parsePath(elem)
    for child in elem.sub:
      findPaths(child)
if __name__ == "__main__":
  if len(sys.argv) != 2:
    print "Give a file yo"
    exit(0)
  filename = sys.argv[1]
  drawing = load(filename)
  findPaths(drawing)
  index = 0
  prefix = filename.rpartition(".")[0]
  # group_file = open(prefix + ".group", "w")
  for path in output_paths:
    outname = prefix + ".path"
    if len(output_paths) > 1:
      outname = prefix + "_shape" + str(index) + ".path"
    index+=1
    path.write(outname)
    # group_file.write(outname + "\n")
  # group_file.close()
