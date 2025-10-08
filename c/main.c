#include <stdlib.h>
#include <time.h>
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "bmp_lib.h"
#include "utils.h"
#include "queue.h"

int MAP_SIZE = 100;
float MAP_DENSITY = 0.5;
int MAP_AMPLITUDE = 48;

pixel *gen_map()
{
    pixel(*pixels)[MAP_SIZE] = malloc_matrix(MAP_SIZE, MAP_SIZE, pixel);
    for (int i = 0; i < MAP_SIZE; i++)
    {
        for (int j = 0; j < MAP_SIZE; j++)
        {
            float r = (float)rand() / (float)RAND_MAX;
            if (r <= MAP_DENSITY)
            {
                pixels[i][j].r = (float)rand() / (float)RAND_MAX * (float)MAP_AMPLITUDE;
                pixels[i][j].g = 0;
                pixels[i][j].b = 0;
            }
            else
            {
                pixels[i][j].r = 0;
                pixels[i][j].g = 0;
                pixels[i][j].b = 0;
            }
        }
    }
    blur((pixel *)pixels, MAP_SIZE, MAP_SIZE);
    return (pixel *)pixels;
}

// 30 degrees
#define MAX_ANGLE (M_PI / 6)
// from color values, to meters
#define height(x) (x * 3.f)
#define length(x) (x * 0.1f)

// starting right, then clockwise
const int di4[] = {0, 1, 0, -1};
const int dj4[] = {1, 0, -1, 0};
const int di8[] = {0, 1, 1, 1, 0, -1, -1, -1};
const int dj8[] = {1, 1, 0, -1, -1, -1, 0, 1};
#define di(n) (n == 4 ? di4 : di8)
#define dj(n) (n == 4 ? dj4 : dj8)

#define ok_pos(p) (in_range(new_pos.i, 0, MAP_SIZE - 1) && in_range(new_pos.j, 0, MAP_SIZE - 1))

bool ok_slope(position p1, position p2, float *map)
{
    float (*map_matrix)[MAP_SIZE] = (float (*)[MAP_SIZE])map;
    float i = fabs(p1.i - p2.i);
    float j = fabs(p1.j - p2.j);
    float x = length(sqrt(i * i + j * j));
    float y = height(fabs(map_matrix[p1.i][p1.j] - map_matrix[p2.i][p2.j]));

    float angle = atan(y / x);
    // printf("angle: %.2f\n", RAD_TO_DEG(angle));
    return angle < MAX_ANGLE;
}

int main(int argc, char *argv[])
{
    srand(time(0)); // initializing the RNG

    bool read_map_from_file = false;
    char *map_file_name = NULL;
    int dir = 4;
    for (int i = 1; i < argc; i++)
    {
        char *arg = argv[i];
        if (strcmp(arg, "--help") == 0 || strcmp(arg, "-h") == 0)
        {
            printf("Format: {command};{arguments{:default}}: {description}\n");
            printf("--help, -h;: prints this help screen\n");
            printf("--map, -m;{file_name}: provide a pre-generated map. The image MUST be square, and the size must be specified explicitly if it's not 100x100. if not specified, a new map is generated and saved to map.bmp\n");
            printf("--mapsize, -s;{int:%d}: sets the map size in px\n", MAP_SIZE);
            printf("--mapdens, -d;{float:%.2f}: sets the map density\n", MAP_DENSITY);
            printf("--mapamplitude, -a;{int[0-255]:%d}: sets the map amplitude\n", MAP_AMPLITUDE);
            printf("-8d;: sets the movement to 8-directional, instead of 4\n");
            printf("Example: ./main --mapsize 50 -d 0.02\n");
            printf("Example: ./main -m map.bmp -8d\n");
            return 0;
        }
        else if (strcmp(arg, "--map") == 0 || strcmp(arg, "-m") == 0)
        {
            if (i + 1 < argc)
            {
                read_map_from_file = true;
                map_file_name = argv[++i];
            }
            else
            {
                printf("Map file name must be provided after %s!\n", arg);
                return 0;
            }
        }
        else if (strcmp(arg, "--mapsize") == 0 || strcmp(arg, "-s") == 0)
        {
            if (i + 1 < argc)
            {
                char *num = argv[++i];
                stoi_error_t err = my_stoi(num, &MAP_SIZE);
                if (err != STOI_SUCCESS)
                {
                    printf("Invalid map size\n");
                    return 0;
                }
            }
            else
            {
                printf("Map size must be provided after %s!\n", arg);
                return 0;
            }
        }
        else if (strcmp(arg, "--mapdens") == 0 || strcmp(arg, "-d") == 0)
        {
            if (i + 1 < argc)
            {
                char *num = argv[++i];
                stof_error_t err = my_stof(num, &MAP_DENSITY);
                if (err != STOF_SUCCESS)
                {
                    printf("Invalid map density\n");
                    return 0;
                }
            }
            else
            {
                printf("Map size must be provided after %s!\n", arg);
                return 0;
            }
        }
        else if (strcmp(arg, "--mapamplitude") == 0 || strcmp(arg, "-a") == 0)
        {
            if (i + 1 < argc)
            {
                char *num = argv[++i];
                stoi_error_t err = my_stoi(num, &MAP_AMPLITUDE);
                if (err != STOI_SUCCESS)
                {
                    printf("Invalid map amplitude\n");
                    return 0;
                }
            }
            else
            {
                printf("Map amplitude must be provided after %s!\n", arg);
                return 0;
            }
        }
        else if (strcmp(arg, "-8d") == 0)
        {
            dir = 8;
        }
    }

    pixel(*pixels)[MAP_SIZE];
    if (!read_map_from_file)
    {
        pixels = (pixel(*)[MAP_SIZE])gen_map();

        write_image_data((pixel *)pixels, MAP_SIZE, MAP_SIZE, "map.bmp");
    }
    else
    {
        pixels = malloc_matrix(MAP_SIZE, MAP_SIZE, pixel);
        read_image_data((pixel *)pixels, map_file_name);
    }

    // Actually solving the problem
    float (*map)[MAP_SIZE] = malloc_matrix(MAP_SIZE, MAP_SIZE, float);
    for (int i = 0; i < MAP_SIZE; i++)
    {
        for (int j = 0; j < MAP_SIZE; j++)
        {
            map[i][j] = pixels[i][j].r / 255.f;
        }
    }

    const position dest = (position){MAP_SIZE - 1, MAP_SIZE - 1};
    const position init = (position){0, 0};
    position(*visited)[MAP_SIZE] = malloc_matrix(MAP_SIZE, MAP_SIZE, position);
#define NOT_VISITED ((position){-1, -1})
    for (int i = 0; i < MAP_SIZE; i++)
    {
        for (int j = 0; j < MAP_SIZE; j++)
        {
            visited[i][j] = NOT_VISITED;
        }
    }
    pq positions;
    init_pq(&positions, MAP_SIZE * MAP_SIZE);
    pq_push(&positions, (position){0, 0});
    visited[init.i][init.j] = (position){-2, -2};
    bool done = false;
    while (!is_pq_empty(&positions) && !done)
    {
        position now = pq_pop(&positions);
        for (int i = 0; i < dir; i++)
        {
            position new_pos = (position){now.i + di(dir)[i], now.j + dj(dir)[i]};
            if (ok_pos(new_pos) && pos_eq(visited[new_pos.i][new_pos.j], NOT_VISITED) && ok_slope(now, new_pos, (float *)map))
            {
                pq_push(&positions, new_pos);
                visited[new_pos.i][new_pos.j] = now;
                if (pos_eq(new_pos, dest))
                {
                    done = true;
                    break;
                }
            }
        }
    }

    if (done)
    {
        position *path = malloc(MAP_SIZE * MAP_SIZE * sizeof(position));

        int path_size = 0;
        for (int i = 0; i < MAP_SIZE * MAP_SIZE; i++)
        {
            path[i] = NOT_VISITED;
        }

        position curr_pos = dest;
        while (pos_neq(curr_pos, ((position){-2, -2})))
        {
            path[path_size++] = curr_pos;
            curr_pos = visited[curr_pos.i][curr_pos.j];
        }

        printf("path size: %d\n", path_size);
        for (int i = path_size - 1; i >= 0; i--)
        {
            printf("%d: (%d; %d)\n", path_size - i, path[i].i, path[i].j);
            pixels[path[i].i][path[i].j].b = 0xFF;
        }

        write_image_data((pixel *)pixels, MAP_SIZE, MAP_SIZE, "path.bmp");

        free(path);
    }
    else
    {
        printf("No path avaliable");
    }
    free(visited);
    free_pq(&positions);
    free(map);
    free(pixels);
    return 0;
}
