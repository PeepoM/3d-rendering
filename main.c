#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

#define WIDTH 80
#define HEIGHT 43

typedef struct
{
    float x, y, z;
} Vector3D;

typedef struct
{
    int x, y;
} Point;

typedef struct
{
    char c;
    Vector3D vertices[1600];
} Face;

static float focal_dist;

Point project_vector(Vector3D world_point)
{
    Vector3D projected = {
        .x = world_point.x / (world_point.z * focal_dist),
        .y = world_point.y / (world_point.z * focal_dist)};

    Point canvas_point = {
        .x = (int)((-projected.x + 1) / 2 * WIDTH),
        .y = (int)((projected.y + 1) / 2 * HEIGHT)};

    return canvas_point;
}

void vector_add(Vector3D a, Vector3D b, Vector3D *result)
{
    result->x = a.x + b.x;
    result->y = a.y + b.y;
    result->z = a.z + b.z;
}

void dump(char buffer[HEIGHT][WIDTH])
{
    for (size_t i = 0; i < HEIGHT; i++)
    {
        for (size_t j = 0; j < WIDTH; j++)
        {
            putchar(buffer[i][j]);
        }
        putchar('\n');
    }
}

int main(void)
{
    Face front, back, top, bottom, left, right;

    size_t c = 0;
    for (float i = -10; i < 10; i += 0.5)
    {
        for (float j = -10; j < 10; j += 0.5)
        {
            Vector3D v_front = {.x = i, .y = j, .z = 10};
            front.vertices[c] = v_front;

            Vector3D v_back = {.x = i, .y = j, .z = -10};
            back.vertices[c] = v_back;

            Vector3D v_top = {.x = i, .y = 10, .z = j};
            top.vertices[c] = v_top;

            Vector3D v_bottom = {.x = i, .y = -10, .z = j};
            bottom.vertices[c] = v_bottom;

            Vector3D v_left = {.x = -10, .y = i, .z = j};
            left.vertices[c] = v_left;

            Vector3D v_right = {.x = 10, .y = i, .z = j};
            right.vertices[c] = v_right;

            c++;
        }
    }
    front.c = '@';
    back.c = '$';
    top.c = '~';
    bottom.c = '#';
    left.c = ';';
    right.c = '+';

    const int n_faces = 6;
    Face faces[n_faces] = {front, back, top, bottom, left, right};

    char buffer[HEIGHT][WIDTH];
    float z_buffer[HEIGHT][WIDTH];

    float alpha = 0;
    focal_dist = tan(0.5 * 90 * M_PI / 180);
    Vector3D offset = {.x = 0, .y = 0, .z = -45};
    float cosa, sina, x, y, z;

    while (true)
    {
        memset(buffer, '.', sizeof buffer);
        memset(z_buffer, -10000, sizeof z_buffer);

        cosa = cos(alpha),
        sina = sin(alpha);

        for (size_t i = 0; i < n_faces; i++)
        {
            Face face = faces[i];
            for (size_t j = 0; j < c; j++)
            {
                x = face.vertices[j].x, y = face.vertices[j].y, z = face.vertices[j].z;
                Vector3D vertex_trans = {
                    .x = x * cosa - y * sina * cosa + z * sina * sina,
                    .y = x * sina + y * cosa * cosa - z * cosa * sina,
                    .z = y * sina + z * cosa};
                vector_add(vertex_trans, offset, &vertex_trans);
                Point point = project_vector(vertex_trans);
                if (vertex_trans.z > z_buffer[point.y][point.x])
                {
                    z_buffer[point.y][point.x] = vertex_trans.z;
                    buffer[point.y][point.x] = face.c;
                }
            }
        }

        printf("\x1b[H\x1b[J");
        dump(buffer);
        alpha += 2 * M_PI / 360;
        usleep(15000);
    }

    return 0;
}
