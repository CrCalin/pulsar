from PIL import Image, ImageFilter
from sys import argv
from position import *
from utils import angle, ok_angle
from heapq import heappush, heappop
import random

map_size = 100
map_density = 0.5
map_amplitude = 48

def gen_map() -> Image.Image:
    global map_size
    global map_density
    global map_amplitude
    
    map = Image.new("RGB", (map_size, map_size), (0, 0, 0))
    map_pixels = map.load()
    
    for i in range(map_size):
        for j in range(map_size):
            r = random.random()
            if r <= map_density:
                map_pixels[i, j] = (random.randint(0, map_amplitude), 0, 0)
            else:
                map_pixels[i, j] = (0, 0, 0) # yes, i know they're already initialized to 0. I'm doing this just to be explicit
    map = map.filter(ImageFilter.GaussianBlur(radius=2))
    return map

def cost(p1: Position, p2: Position, map: list[list[float]]) -> float:
    return angle(p1, p2, map) + p1.distance_to(p2) / 10.0

def main():
    global map_size
    global map_density
    global map_amplitude
    
    read_map_from_file = False
    map_file_name = ""
    dir = 4
    format = "png"
    
    i = 1
    while i < len(argv):
        arg = argv[i]
        if arg == '--help' or arg == '-h':
            print(f"Format::{{command}};{{arguments{{:default}}}}::{{description}}")
            print(f"--help, -h;: prints this help screen")
            print(f"--format, -f;{{\"bmp\"|\"png\"}}: set the format to use for a newly generated map and for the output path")
            print(f"--map, -m;{{file_name}}: provide a pre-generated map. The image MUST be square. if not specified, a new map is generated and saved to map.bmp")
            print(f"--mapsize, -s;{{int:{map_size}}}: sets the map size in px")
            print(f"--mapdens, -d;{{float:{map_density:.2f}}}: sets the map density")
            print(f"--mapamplitude, -a;{{int[0-255]:{map_amplitude}}}: sets the map amplitude")
            print(f"-8d;: sets the movement to 8-directional, instead of 4")
            print(f"Example: ./main --mapsize 50 -d 0.02")
            print(f"Example: ./main -m map.bmp -8d")
            return
        elif arg == "--format" or arg == "-f":
            if i + 1 < len(argv):
                format = argv[i + 1].clone()
                i += 1
            else:
                print("Format must be provided after {arg}! Check --help for avaliable options!")
                return
        elif arg == "--map" or arg == "-m":
            if i + 1 < len(argv):
                read_map_from_file = True
                map_file_name = argv[i + 1]
                i += 1
            else:
                print("Map file name must be provided after {arg}!");
                return
        elif arg == "--mapsize" or arg == "-s":
            if i + 1 < len(argv):
                num = argv[i + 1]
                i += 1
                map_size = int(num)
            else:
                print("Map size must be provided after {arg}!")
                return
        elif arg == "--mapdens" or arg == "-d":
            if i + 1 < len(argv):
                num = argv[i + 1]
                i += 1
                map_density = float(num)
            else:
                print("Map size must be provided after {arg}!")
                return
        elif arg == "--mapamplitude" or arg == "-a":
            if i + 1 < len(argv):
                num = argv[i + 1]
                i += 1
                map_amplitude = int(num)
            else:
                print("Map amplitude must be provided after {arg}!")
                return
        elif arg == "-8d":
            dir = 8
        else:
            print("Unknown arg::arg}")
            return
        i += 1

    pixels = None
    image: Image.Image
    if not read_map_from_file:
        image = gen_map()
        image.save("map.png")
        pixels = image.load()
    else:
        image = Image.open(map_file_name).convert("RGB")
        if image.width != map_size:
            print("Image must be exactly square")
            return
        pixels = image.load()
        map_size = image.width
        
    map = [[pixels[i, j][0] / 255.0 for j in range(map_size)] for i in range(map_size)] # extracting just the red channel
            
    dest = Position(map_size - 1, map_size - 1)
    init = Position(0, 0)
    
    visited: list[list[Position]] = [[None for j in range(map_size)] for i in range(map_size)]
    distance: list[list[float]] = [[float('inf') for j in range(map_size)] for i in range(map_size)]
    distance[init.i][init.j] = 0.0
    pq = [] # heapq
    heappush(pq, (0, init, init)) # len, pos, from
    while len(pq) > 0:
        (length, pos, pos_from) = heappop(pq)
        if visited[pos.i][pos.j] != None:
            continue
        visited[pos.i][pos.j] = pos_from
        
        if pos == dest:
            break
                
        for i in range(dir):
            new_pos = pos + get_d(dir, i)
            if in_bounds(new_pos, 0, map_size - 1) and ok_angle(pos, new_pos, map):
                new_dist = length + cost(pos, new_pos, map)
                if new_dist < distance[new_pos.i][new_pos.j]:
                    distance[new_pos.i][new_pos.j] = new_dist
                    heappush(pq, (new_dist, new_pos, pos))
    
    if distance[dest.i][dest.j] != float('inf'):
        path: list[Position] = []
        
        old_pos: Position = None
        curr_pos: Position = dest
        while curr_pos != old_pos:
            path.append(curr_pos)
            old_pos = curr_pos
            curr_pos = visited[curr_pos.i][curr_pos.j]
        
        print(f"Path size: {len(path)}");
        for i in range(len(path) - 1, -1, -1):
            print(f"{len(path) - i}: ({path[i].i}; {path[i].j})")
            r, g, _ = pixels[path[i].i, path[i].j]
            pixels[path[i].i, path[i].j] = (r, g, 255)
            
        image.save(f"path.{format}")
    else:
        print("No path avaliavle")


if __name__ == "__main__":
    main()