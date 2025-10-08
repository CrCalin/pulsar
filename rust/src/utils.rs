use crate::position::Position;
use std::f32::consts::PI;

fn length(x: f32) -> f32 {
    x * 0.1
}
fn height(x: f32) -> f32 {
    x * 3.0
}
pub fn angle(p1: Position, p2: Position, map: &Vec<Vec<f32>>) -> f32 {
    let x = length(p1.distance_to(&p2));
    let y = height((map[p1] - map[p2]).abs());

    (y/x).atan()
}
const MAX_ANGLE: f32 = PI / 6.0;
pub fn ok_angle(p1: Position, p2: Position, map: &Vec<Vec<f32>>) -> bool {
    angle(p1, p2, map) <= MAX_ANGLE
}