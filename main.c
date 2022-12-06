#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

#define WIDTH 80
#define HEIGHT 43

static float focal_dist;

typedef struct {
    float x, y, z;
} Vector3D;

typedef struct {
    int x, y;
} Point;

void project_vector(const Vector3D *world_point, Point *canvas_point) {
    Vector3D projected = {
            .x = world_point->x / (world_point->z * focal_dist),
            .y = world_point->y / (world_point->z * focal_dist)};

    canvas_point->x = (int) ((-projected.x + 1) / 2 * WIDTH);
    canvas_point->y = (int) ((projected.y + 1) / 2 * HEIGHT);
}

void vector_add(const Vector3D *a, const Vector3D *b, Vector3D *result) {
    result->x = a->x + b->x;
    result->y = a->y + b->y;
    result->z = a->z + b->z;
}

void dump(char buffer[HEIGHT][WIDTH]) {
    for (size_t i = 0; i < HEIGHT; i++) {
        for (size_t j = 0; j < WIDTH; j++) {
            putchar(buffer[i][j]);
        }
        putchar('\n');
    }
}

void transform(Vector3D *vector, float rot_angle, const Vector3D *z_offset) {
    float x = vector->x, y = vector->y, z = vector->z, cosa = cosf(rot_angle), sina = sinf(rot_angle);

    vector->x = x * cosa - y * sina * cosa + z * sina * sina;
    vector->y = x * sina + y * cosa * cosa - z * cosa * sina;
    vector->z = y * sina + z * cosa;

    vector_add(vector, z_offset, vector);
}

int main(void) {
    char buffer[HEIGHT][WIDTH];
    float z_buffer[HEIGHT][WIDTH];

    float edge_len = 20, spacing = 0.5f, rot_angle = 0, start = -edge_len / 2, end = edge_len / 2;
    Vector3D z_offset = {.x = 0, .y = 0, .z = -35};
    focal_dist = tanf(0.5 * 90 * M_PI / 180);

    while (true) {
        memset(buffer, ' ', sizeof buffer);
        memset(z_buffer, -10000, sizeof z_buffer);

        for (float i = start; i < end; i += spacing) {
            for (float j = start; j < end; j += spacing) {
                Vector3D v_front = {.x = i, .y = j, .z = end};
                Vector3D v_back = {.x = i, .y = j, .z = start};
                Vector3D v_top = {.x = i, .y = end, .z = j};
                Vector3D v_bottom = {.x = i, .y = start, .z = j};
                Vector3D v_left = {.x = start, .y = i, .z = j};
                Vector3D v_right = {.x = end, .y = i, .z = j};

                Vector3D *vectors[6] = {&v_front, &v_back, &v_top, &v_bottom, &v_left, &v_right};
                char *symbols = "@$~#;+";
                for (size_t k = 0; k < 6; k++) {
                    transform(vectors[k], rot_angle, &z_offset);
                    Point point;
                    project_vector(vectors[k], &point);
                    if (vectors[k]->z > z_buffer[point.y][point.x]) {
                        z_buffer[point.y][point.x] = vectors[k]->z;
                        buffer[point.y][point.x] = symbols[k];
                    }
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
