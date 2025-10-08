from position import Position

def length(x: float) -> float: return x * 0.1
def height(x: float) -> float: return x * 3

from math import atan, pi
def angle(p1: Position, p2: Position, map: list[list[float]]) -> float:
    x = length(p1.distance_to(p2))
    y = height(abs(map[p1.i][p1.j] - map[p2.i][p2.j]))
    
    return atan(y / x)
MAX_ANGLE = pi / 6.0
def ok_angle(p1: Position, p2: Position, map: list[list[float]]) -> bool:
    return angle(p1, p2, map) <= MAX_ANGLE