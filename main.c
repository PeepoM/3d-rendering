#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

#define WIDTH 80
#define HEIGHT 43

static const int n_vertices;
static float focal_dist;

typedef struct {
    float x, y, z;
} Vector3D;

typedef struct {
    int x, y;
} Point;

typedef struct {
    char c;
    Vector3D vertices[1600];
} Face;

Point project_vector(Vector3D world_point) {
    Vector3D projected = {
            .x = world_point.x / (world_point.z * focal_dist),
            .y = world_point.y / (world_point.z * focal_dist)};

    Point canvas_point = {
            .x = (int) ((-projected.x + 1) / 2 * WIDTH),
            .y = (int) ((projected.y + 1) / 2 * HEIGHT)};

    return canvas_point;
}

void vector_add(Vector3D a, Vector3D b, Vector3D *result) {
    result->x = a.x + b.x;
    result->y = a.y + b.y;
    result->z = a.z + b.z;
}

void dump(char buffer[HEIGHT][WIDTH]) {
    for (size_t i = 0; i < HEIGHT; i++) {
        for (size_t j = 0; j < WIDTH; j++) {
            putchar(buffer[i][j]);
        }
        putchar('\n');
    }
}

int main(void) {
    float edge_length = 20, spacing = 0.5f;
    unsigned int num_vertices = (int) powf(edge_length / spacing, 2);

    // construct each face of the cube
    Face front, back, top, bottom, left, right;
    size_t c = 0;
    for (float i = 0; i < edge_length; i += spacing) {
        for (float j = 0; j < edge_length; j += spacing) {
            Vector3D v_front = {.x = i, .y = j, .z = edge_length};
            front.vertices[c] = v_front;

            Vector3D v_back = {.x = i, .y = j, .z = 0};
            back.vertices[c] = v_back;

            Vector3D v_top = {.x = i, .y = edge_length, .z = j};
            top.vertices[c] = v_top;

            Vector3D v_bottom = {.x = i, .y = 0, .z = j};
            bottom.vertices[c] = v_bottom;

            Vector3D v_left = {.x = 0, .y = i, .z = j};
            left.vertices[c] = v_left;

            Vector3D v_right = {.x = edge_length, .y = i, .z = j};
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

    Face faces[6] = {front, back, top, bottom, left, right};
    Vector3D offset = {.x = -edge_length / 2, .y = -edge_length / 2, .z = -edge_length / 2};
    for (size_t i = 0; i < 6; i++) {
        Vector3D *vertices = faces[i].vertices;
        for (size_t j = 0; j < num_vertices; j++) {
            vector_add(vertices[j], offset, &(vertices[j]));
        }
    }

    char buffer[HEIGHT][WIDTH];
    float z_buffer[HEIGHT][WIDTH];

    Vector3D z_offset = {.x = 0, .y = 0, .z = -35};

    float rot_angle = 0;
    focal_dist = tanf(0.5 * 90 * M_PI / 180);

    while (true) {
        memset(buffer, '.', sizeof buffer);
        memset(z_buffer, -10000, sizeof z_buffer);

        float cosa = cosf(rot_angle), sina = sinf(rot_angle);

        for (size_t i = 0; i < 6; i++) {
            Face face = faces[i];
            for (size_t j = 0; j < c; j++) {
                float x = face.vertices[j].x, y = face.vertices[j].y, z = face.vertices[j].z;
                Vector3D vertex_trans = {
                        .x = x * cosa - y * sina * cosa + z * sina * sina,
                        .y = x * sina + y * cosa * cosa - z * cosa * sina,
                        .z = y * sina + z * cosa};
                vector_add(vertex_trans, z_offset, &vertex_trans);
                Point point = project_vector(vertex_trans);
                if (vertex_trans.z > z_buffer[point.y][point.x]) {
                    z_buffer[point.y][point.x] = vertex_trans.z;
                    buffer[point.y][point.x] = face.c;
                }
            }
        }

        // clear screen and display buffer contents
        printf("\x1b[H\x1b[J");
        dump(buffer);

        rot_angle += 2 * M_PI / 360;
        usleep(15000);
    }

    return 0;
}
