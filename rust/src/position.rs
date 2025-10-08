use std::cmp::Ordering;
use std::ops::{Add, Index, IndexMut};

#[derive(Clone, Copy, PartialEq, Eq, Ord, PartialOrd)]
pub struct Position {
    pub i: i32, pub j: i32
}

impl Position {
    pub fn distance_to(&self, other: &Position) -> f32 {
        let i = (self.i - other.i).abs() as f32;
        let j = (self.j - other.j).abs() as f32;

        (i*i + j*j).sqrt()
    }
}

impl<T> Index<Position> for Vec<Vec<T>> {
    type Output = T;

    fn index(&self, index: Position) -> &Self::Output {
        &self[index.i as usize][index.j as usize]
    }
}
impl<T> IndexMut<Position> for Vec<Vec<T>> {
    fn index_mut(&mut self, index: Position) -> &mut Self::Output {
        &mut self[index.i as usize][index.j as usize]
    }
}

#[derive(PartialEq)]
pub struct LenPos {
    pub len: f32, 
    pub pos: Position,
    pub from: Position
}
impl Eq for LenPos {}
impl PartialOrd for LenPos {
    fn partial_cmp(&self, other: &Self) -> Option<Ordering> {
        Some(match self.len.partial_cmp(&other.len)? {
            Ordering::Less => Ordering::Greater,
            Ordering::Equal => Ordering::Equal,
            Ordering::Greater => Ordering::Less,
        }) // swapping the order because std::collections::BinaryHeap is a maxheap, and we want shortest lengths first
    }
}
impl Ord for LenPos {
    fn cmp(&self, other: &Self) -> Ordering {
        self.partial_cmp(other).unwrap()
    }
}

impl Add for Position {
    type Output = Position;

    fn add(self, rhs: Self) -> Self::Output {
        Position{i: self.i + rhs.i, j: self.j + rhs.j}
    }
}

// Starting right, then clockwise
const D4: [Position; 4] = [
    Position{i: 0, j: 1},
    Position{i: 1, j: 0},
    Position{i: 0, j: -1},
    Position{i: -1, j: 0},
];
const D8: [Position; 8] = [
    Position{i: 0, j: 1},
    Position{i: 1, j: 1},
    Position{i: 1, j: 0},
    Position{i: 1, j: -1},
    Position{i: 0, j: -1},
    Position{i: -1, j: -1},
    Position{i: -1, j: 0},
    Position{i: -1, j: 1},
];
pub fn get_d(nr_dir: usize, i: usize) -> Position {
    match nr_dir {
        4 => D4[i],
        8 => D8[i],
        _ => panic!("Number of directions should only ever be 4 or 8")
    }
}

pub fn in_bounds(pos: Position, low_bound: i32, high_bound: i32) -> bool {
    (low_bound <= pos.i && pos.i <= high_bound) && (low_bound <= pos.j && pos.j <= high_bound)
}
