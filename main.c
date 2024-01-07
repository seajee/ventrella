#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <raylib.h>
#include <raymath.h>

#define RADIUS 15
#define RED_PARTICLES 100
#define BLUE_PARTICLES 10000

typedef struct
{
    Vector2 pos;
    Vector2 vel;
    Color col;
} Particle;

typedef struct
{
    Particle* particles;
    size_t count;
} Cluster;

Cluster cluster_alloc(size_t count, Color color)
{
    Cluster cluster = {0};
    cluster.particles = (Particle*)malloc(sizeof(Particle) * count);
    if (cluster.particles == NULL) {
        return cluster;
    }
    cluster.count = count;

    int width = GetScreenWidth();
    int height = GetScreenHeight();

    for (size_t i = 0; i < count; ++i) {
        cluster.particles[i].pos.x = rand() % width;
        cluster.particles[i].pos.y = rand() % height;
        cluster.particles[i].vel.x = 0;
        cluster.particles[i].vel.y = 0;
        cluster.particles[i].col = color;
    }

    return cluster;
}

void cluster_free(Cluster cluster)
{
    free(cluster.particles);
}

void process_cluster(Cluster first, Cluster second, float g, float max_distance)
{
    int width = GetScreenWidth();
    int height = GetScreenHeight();

    for (size_t i = 0; i < first.count; ++i) {
        Vector2 f = Vector2Zero();
        Particle* a = &first.particles[i];

        for (size_t j = 0; j < second.count; ++j) {
            Particle b = second.particles[j];

            Vector2 dv = {
                .x = a->pos.x - b.pos.x,
                .y = a->pos.y - b.pos.y
            };
            float d = sqrtf(dv.x*dv.x + dv.y*dv.y);

            if (d > 0.0f && d < max_distance) {
                float force = g * 1.0f/d;
                f = Vector2Add(f, Vector2Scale(dv, force));
            }

            a->vel = Vector2Scale(Vector2Add(a->vel, f), 0.5f);
            a->pos = Vector2Add(a->pos, a->vel);

            if (a->pos.x <= RADIUS || a->pos.x >= width - RADIUS) a->vel.x *= -1.0f;
            if (a->pos.y <= RADIUS || a->pos.y >= height - RADIUS) a->vel.y *= -1.0f;
        }
    }
}

int main(void)
{
    srand(time(NULL));
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 600, "ventrella");

    Cluster red_cluster = cluster_alloc(RED_PARTICLES, RED);
    if (red_cluster.particles == NULL) {
        fprintf(stderr, "ERROR: Could not allocate cluster\n");
        return EXIT_FAILURE;
    }

    Cluster blue_cluster = cluster_alloc(BLUE_PARTICLES, BLUE);
    if (blue_cluster.particles == NULL) {
        fprintf(stderr, "ERROR: Could not allocate cluster\n");
        return EXIT_FAILURE;
    }

    bool pause = true;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        if (IsKeyPressed(KEY_R)) {
            cluster_free(red_cluster);
            cluster_free(blue_cluster);

            red_cluster = cluster_alloc(RED_PARTICLES, RED);
            if (red_cluster.particles == NULL) {
                fprintf(stderr, "ERROR: Could not allocate cluster\n");
                return EXIT_FAILURE;
            }

            blue_cluster = cluster_alloc(BLUE_PARTICLES, BLUE);
            if (blue_cluster.particles == NULL) {
                fprintf(stderr, "ERROR: Could not allocate cluster\n");
                return EXIT_FAILURE;
            }
        }

        if (IsKeyPressed(KEY_SPACE)) {
            pause = !pause;
        }

        if (!pause) {
            process_cluster(red_cluster, red_cluster, -1.0f*dt, 80.0f);
            process_cluster(red_cluster, blue_cluster, 0.1f*dt, 19.0f);
            process_cluster(blue_cluster, red_cluster, 0.1f*dt, 80.0f);
        }

        BeginDrawing();
        ClearBackground(GetColor(0x181818AA));

        for (size_t i = 0; i < red_cluster.count; ++i) {
            Particle* it = &red_cluster.particles[i];
            //DrawCircleV(it->pos, RADIUS, it->col);
            DrawPixelV(it->pos, it->col);
        }
        for (size_t i = 0; i < blue_cluster.count; ++i) {
            Particle* it = &blue_cluster.particles[i];
            //DrawCircleV(it->pos, RADIUS, it->col);
            DrawPixelV(it->pos, it->col);
        }

        EndDrawing();
    }

    CloseWindow();
    cluster_free(red_cluster);
    cluster_free(blue_cluster);

    return EXIT_SUCCESS;
}
