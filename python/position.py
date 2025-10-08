from dataclasses import dataclass
from math import sqrt

@dataclass
class Position:
    i: int
    j: int
    
    def distance_to(self, other: "Position") -> float:
        i: float = abs(self.i - other.i)
        j: float = abs(self.j - other.j)
        return sqrt(i*i + j*j)
    
    def __add__(self, other: "Position") -> "Position":
        return Position(self.i + other.i, self.j + other.j)
    
    def __lt__(self, other: "Position") -> bool:
        if self.i != other.i:
            return self.i < other.i
        return self.j < other.j
    
D4 = [
    Position(0, 1),
    Position(1, 0),
    Position(0, -1),
    Position(-1, 0),
]
D8 = [
    Position(0, 1),
    Position(1, 1),
    Position(1, 0),
    Position(1, -1),
    Position(0, -1),
    Position(-1, -1),
    Position(-1, 0),
    Position(-1, 1),
]
def get_d(nr_dir: int, i: int) -> Position:
    match nr_dir:
        case 4:
            return D4[i]
        case 8:
            return D8[i]
        case _:
            raise Exception("Number of directions should only ever be 4 or 8")
        
def in_bounds(pos: Position, low_bound: int, high_bound: int) -> bool:
    return (low_bound <= pos.i and pos.i <= high_bound) and (low_bound <= pos.j and pos.j <= high_bound)