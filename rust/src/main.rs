use std::{collections::BinaryHeap, env::args, f32::INFINITY};

use image::{DynamicImage, ImageBuffer, Rgb, RgbImage};

mod position;
mod utils;
use position::*;
use utils::*;

#[derive(Clone, Copy)]
struct MapParams {
    size: usize,
    density: f32,
    amplitude: u8
}

use rand::Rng;
fn gen_map(map_size: usize, params: MapParams) -> RgbImage {
    let mut rng = rand::rng();

    let mut map = ImageBuffer::new(map_size as u32, map_size as u32);
    for i in 0u32..map_size as u32 {
        for j in 0u32..map_size as u32 {
            let r = rng.random_range(0.0_f32..=1.0f32);
            if r <= params.density {
                map.put_pixel(i, j, Rgb([rng.random_range(0u8..params.amplitude), 0, 0]));
            }
            else {
                map.put_pixel(i, j, Rgb([0, 0, 0])); // yes, i know they're already initialized to 0. I'm doing this just to be explicit
            }
        }
    }
    let img = DynamicImage::ImageRgb8(map).blur(1.0); // random value i picked, can be adjusted (higher = more blurry)

    img.into_rgb8()
}

fn main() {
    let mut map_params = MapParams{size: 100, density: 0.5, amplitude: 48};

    let mut read_map_from_file = false;
    let mut map_file_name = String::from("");
    let mut dir = 4;
    let mut format = String::from("png");

    let args: Vec<String> = args().collect();
    let mut i = 1;
    while i < args.len() {
        let arg = &args[i];
        if arg == "--help" || arg == "-h" {
            println!("Format: {{command}};{{arguments{{:default}}}}: {{description}}");
            println!("--help, -h;: prints this help screen");
            println!("--format, -f;{{\"bmp\"|\"png\"}}: set the format to use for a newly generated map and for the output path");
            println!("--map, -m;{{file_name}}: provide a pre-generated map. if not specified, a new map is generated and saved to map.bmp");
            println!("--mapsize, -s;{{int:{}}}: sets the map size in px", map_params.size);
            println!("--mapdens, -d;{{float:{:.2}}}: sets the map density", map_params.density);
            println!("--mapamplitude, -a;{{int[0-255]:{}}}: sets the map amplitude", map_params.amplitude);
            println!("-8d;: sets the movement to 8-directional, instead of 4");
            println!("Example: ./main --mapsize 50 -d 0.02");
            println!("Example: ./main -m map.bmp -8d");
            return;
        }
        else if arg == "--format" || arg == "-f" {
            if i + 1 < args.len() {
                format = args[i + 1].clone();
                i += 1;
            }
            else {
                println!("Format must be provided after {arg}! Check --help for avaliable options!");
                return;
            }
        }
        else if arg == "--map" || arg == "-m" {
            if i + 1 < args.len() {
                read_map_from_file = true;
                map_file_name = args[i + 1].clone();
                i += 1;
            }
            else {
                println!("Map file name must be provided after {arg}!");
                return;
            }
        }
        else if arg == "--mapsize" || arg == "-s" {
            if i + 1 < args.len() {
                let num = &args[i + 1];
                i += 1;
                map_params.size = match num.parse::<usize>() {
                    Ok(x) => x,
                    Err(_) => { println!("Invalid map size"); return; }
                };
            }
            else {
                println!("Map size must be provided after {arg}!");
                return;
            }
        }
        else if arg == "--mapdens" || arg == "-d" {
            if i + 1 < args.len() {
                let num = &args[i + 1];
                i += 1;
                map_params.density = match num.parse::<f32>() {
                    Ok(x) => x,
                    Err(_) => { println!("Invalid map density"); return; }
                };
            }
            else {
                println!("Map size must be provided after {arg}!");
                return;
            }
        }
        else if arg == "--mapamplitude" || arg == "-a" {
            if i + 1 < args.len() {
                let num = &args[i + 1];
                i += 1;
                map_params.amplitude = match num.parse::<u8>() {
                    Ok(x) => x,
                    Err(_) => { println!("Invalid map amplitude"); return; }
                };
            }
            else {
                println!("Map amplitude must be provided after {arg}!");
                return;
            }
        }
        else if arg == "-8d" {
            dir = 8;
        } 
        else {
            println!("Unknown arg: {arg}");
            return;
        }
        i += 1;
    }

    let mut pixels: RgbImage;
    if !read_map_from_file {
        pixels = gen_map(map_params.size, map_params);
        match pixels.save(format!("map.{format}")) {
            Ok(_) => (),
            Err(_) => {
                println!("Generated map could not be saved. Continuing...");
            }
        };
    } else {
        pixels = image::open(map_file_name).unwrap().to_rgb8();
    }

    let mut map = vec![vec![0_f32; map_params.size]; map_params.size];
    for i in 0..map_params.size {
        for j in 0..map_params.size {
            map[i][j] = pixels.get_pixel(i as u32, j as u32)[0] as f32 / 255_f32;
        }
    }

    let dest: Position = Position { i: map_params.size as i32 - 1, j: map_params.size as i32 - 1};
    let init: Position = Position { i: 0, j: 0};
    
    let mut visited = vec![vec![None; map_params.size]; map_params.size];
    let mut distance = vec![vec![INFINITY; map_params.size]; map_params.size];
    distance[init] = 0.0;
    let mut q = BinaryHeap::new();
    q.push(LenPos{len: 0.0, pos: init, from: init});
    // Dijkstra's using the absolute value of the angle as a cost map
    while !q.is_empty() {
        let LenPos{len, pos, from} = q.pop().unwrap();
        if visited[pos] != None {
            continue;
        }
        visited[pos] = Some(from);

        if pos == dest {
            break;
        }

        for i in 0usize..dir {
            let new_pos = pos + get_d(dir, i);
            if in_bounds(new_pos, 0, map_params.size as i32 - 1) && ok_angle(pos, new_pos, &map) {
                let new_dist = len + angle(pos, new_pos, &map);
                if new_dist < distance[new_pos] {
                    distance[new_pos] = new_dist;
                    q.push(LenPos { len: new_dist, pos: new_pos, from: pos });
                }
            }
        }
    }

    if distance[dest] != INFINITY {
        let mut path: Vec<Position> = vec![];

        let mut old_pos = None;
        let mut curr_pos = dest;
        while Some(curr_pos) != old_pos {
            path.push(curr_pos);
            old_pos = Some(curr_pos);
            curr_pos = visited[curr_pos].unwrap();
        }

        println!("Path size: {}", path.len());
        for i in (0..path.len()).rev() {
            println!("{}: ({}; {})", path.len() - i, path[i].i, path[i].j);
            pixels.get_pixel_mut(path[i].i as u32, path[i].j as u32)[2] = 0xFF;
        }

        pixels.save(format!("path.{format}")).unwrap();
    } else {
        println!("No path avaliable");
    }
    
}
