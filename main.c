#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <unistd.h>

#define WIDTH 80
#define HEIGHT 24

typedef struct {
    double x, y, z;
} Vector3D;

typedef struct {
    double x, y;
} Vector2D;

typedef struct {
    int x, y;
} Point;

static double focal_dist;

Point project_vector(Vector3D world_point) {
    Vector3D projected = {
            .x = world_point.x / (world_point.z * focal_dist),
            .y = world_point.y / (world_point.z * focal_dist)};

    Point canvas_point = {
            .x = (int) ((-projected.x + 1) / 2 * WIDTH),
            .y = (int) ((projected.y + 1) / 2 * HEIGHT)};

    return canvas_point;
}

void mat_vect_mult(double matrix[][3], Vector3D vector, Vector3D *result) {
    result->x = matrix[0][0] * vector.x + matrix[0][1] * vector.y + matrix[0][2] * vector.z;
    result->y = matrix[1][0] * vector.x + matrix[1][1] * vector.y + matrix[1][2] * vector.z;
    result->z = matrix[2][0] * vector.x + matrix[2][1] * vector.y + matrix[2][2] * vector.z;
}

void vector_add(Vector3D a, Vector3D b, Vector3D *result) {
    result->x = a.x + b.x;
    result->y = a.y + b.y;
    result->z = a.z + b.z;
}

double dotProduct(Vector3D vector_a, Vector3D vector_b) {
    return vector_a.x * vector_b.x +
           vector_a.y * vector_b.y +
           vector_a.z * vector_b.z;
}

void crossProduct(Vector3D vector_a, Vector3D vector_b, Vector3D *cross) {
    cross->x = vector_a.y * vector_b.z - vector_a.z * vector_b.y;
    cross->y = vector_a.z * vector_b.x - vector_a.x * vector_b.z;
    cross->z = vector_a.x * vector_b.y - vector_a.y * vector_b.x;
}

void clear_buffer(char buffer[HEIGHT][WIDTH]) {
    for (size_t i = 0; i < HEIGHT; i++) {
        for (size_t j = 0; j < WIDTH; j++) {
            buffer[i][j] = ' ';
        }
    }
}

void dump(char buffer[HEIGHT][WIDTH]) {
    for (size_t i = 0; i < HEIGHT; i++) {
        for (size_t j = 0; j < WIDTH; j++) {
            putchar(buffer[i][j]);
        }
        putchar('\n');
    }
}

void mat_mat_mult(double mat_a[3][3], double mat_b[3][3], double mat_result[3][3]) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            mat_result[i][j] = 0;
            for (int k = 0; k < 3; k++) {
                mat_result[i][j] += mat_a[i][k] * mat_b[k][j];
            }
        }
    }
}

int main(void) {
    // initialize cube vertices
    Vector3D vertices[200];
    size_t c = 0;

    // front
    for (size_t y = 0; y < 10; y++) {
        for (size_t x = 0; x < 10; x++) {
            Vector3D v = {.x = (double) x, .y = (double) y, .z = 10};
            vertices[c] = v;
            c++;
        }
    }

    // back
    for (size_t y = 0; y < 10; y++) {
        for (size_t x = 0; x < 10; x++) {
            Vector3D v = {.x = (double) x, .y = (double) y, .z = 0};
            vertices[c] = v;
            c++;
        }
    }

    // left

    // right

    Point points[8];
    char buffer[HEIGHT][WIDTH];

    double alpha = 0;
    focal_dist = tan(0.5 * 90 * M_PI / 180);
    Vector3D light = {.x = 0, .y = 0, .z = -1};
    Vector3D offset = {.x = 0, .y = 0, .z = -30};
    bool running = true;
    double rot_result[3][3];

    while (running) {
        printf("\x1b[H\x1b[J");
        clear_buffer(buffer);

        double z_rot[3][3] = {{cos(alpha), -sin(alpha), 0},
                              {sin(alpha), cos(alpha),  0},
                              {0,          0,           1}};
        double x_rot[3][3] = {{1, 0,          0},
                              {0, cos(alpha), -sin(alpha)},
                              {0, sin(alpha), cos(alpha)}};

        mat_mat_mult(z_rot, x_rot, rot_result);

        for (size_t i = 0; i < 200; i++) {
            Vector3D vertex_trans;
            mat_vect_mult(rot_result, vertices[i], &vertex_trans);
            vector_add(vertex_trans, offset, &vertex_trans);
            Point point = project_vector(vertex_trans);
            buffer[point.y - 1][point.x - 1] = '#';
        }

        dump(buffer);
        alpha += 2 * M_PI / 180;
        usleep(25 * 1000);
    }


    return 0;
}
