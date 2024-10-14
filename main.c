#include <math.h>
#include <raylib.h>
#include <raymath.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>

#define BTH_SALLOC_IMPLEMENTATION
#include "bth_salloc.h"

#define WIN_WIDTH  800
#define WIN_HEIGHT 600
#define WIN_CENTER (CLITERAL(Vector2){WIN_WIDTH/2.0,WIN_HEIGHT/2.0})

#define GET_UID(n) NODES[(n)].uid
#define GET_MAP_IDX(n) ((n)/8)
#define GET_MAP_MASK(n) ((n)%8)
#define VISIT(n, m) (m[GET_MAP_IDX(n)] |= (0x01 << GET_MAP_MASK(n)))
#define STATE(n, m) (m[GET_MAP_IDX(n)] & (0x01 << GET_MAP_MASK(n)))
#define R 10.0

struct Node
{
    uint32_t uid;
    Vector2 pos;
    char units;
    uint32_t adjs[8];
};
typedef struct Node Node;

static uint32_t MAP_ORDER = 0;
static Node *NODES;

int mod(int a, int b)
{
    int ret = a % b;

    if (ret < 0)
        ret += b;

    return ret;
}

void RenderNodes(uint32_t start, char **m)
{
    struct Node *s = &NODES[start];
    // VISIT(s->uid, marker);
    m[start][start] = 1;
    Vector2 pos = Vector2Add(s->pos, WIN_CENTER);

    DrawCircleV(pos, R, DARKBLUE);

    for (int i = 0; i < 8; i++)
    {
        uint32_t adj = s->adjs[i];
        if (adj == 0xFFFFFFFF)
            continue;

        if (m[start][adj] == 0 && m[adj][start] == 0)
        {
            Vector2 npos = Vector2Add(NODES[adj].pos, WIN_CENTER);
            DrawLineEx(pos, npos, 2.0, DARKBLUE);
            m[start][s->adjs[i]] = 1;
            m[s->adjs[i]][start] = 1;

            if (m[adj][adj] == 0)
                RenderNodes(s->adjs[i], m);
        }

        // if (!STATE(s->adjs[i], marker))
        // {
        //     RenderNodes(s->adjs[i], marker, emarker);
        // }
    }
}

void InitMap(uint32_t *start)
{
    Node *s = smalloc(sizeof(Node));

    s->uid = MAP_ORDER++;
    s->pos = (Vector2){0.0, 0.0};
    s->units = 0;
    memset(s->adjs, 0xFFFFFFFF, 8 * sizeof(uint32_t));

    NODES = srealloc(NODES, (MAP_ORDER + 8) * sizeof(Node));
    memset(NODES + MAP_ORDER, 0, 8 * sizeof(Node));
    float rad = R * 5;

    for (int i = 0; i < 8; i++)
    {
        uint32_t uid = MAP_ORDER++;
        Vector2 pos = {cos(i*PI/4) * rad, sin(i*PI/4) * rad};

        memset(NODES[uid].adjs, 0xFFFFFFFF, 8 * sizeof(uint32_t));

        NODES[uid].uid = uid;
        NODES[uid].pos = pos;
        NODES[uid].adjs[0] = s->uid;
        NODES[uid].adjs[1] = s->uid + 1 + (uint32_t)mod(i - 1, 8);
        NODES[uid].adjs[2] = s->uid + 1 + (uint32_t)mod(i + 1, 8);
        s->adjs[i] = uid;
    }

    NODES[s->uid] = *s;
    *start = s->uid;
}

int main(void)
{
    SetTraceLogLevel(LOG_FATAL);
    InitWindow(WIN_WIDTH, WIN_HEIGHT, "Bob's R.I.S.K");
    // SetTraceLogLevel(LOG_TRACE);

    uint32_t start;
    InitMap(&start);
    size_t memsize = MAP_ORDER / 8 + ((MAP_ORDER % 8) == 0 ? 0 : 1);
    char **marker = smalloc(MAP_ORDER * sizeof(char *));

    for (uint32_t i = 0; i < MAP_ORDER; i++)
        marker[i] = smalloc(MAP_ORDER * sizeof(char));

    while (!WindowShouldClose())
    {
        for (uint32_t i = 0; i < MAP_ORDER; i++)
            memset(marker[i], 0, MAP_ORDER);

        BeginDrawing();
            ClearBackground(RAYWHITE);
            RenderNodes(start, marker);
        EndDrawing();
    }

    SetTraceLogLevel(LOG_FATAL);
    CloseWindow();
    return 0;
}
