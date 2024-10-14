#include <math.h>
#include <raylib.h>
#include <raymath.h>
#include <stdint.h>
#include <string.h>

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
#define R 15.0

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
   if(ret < (0x7FFFFFFF))
     ret+=b;
   return ret;
}

void RenderNodes(uint32_t start, char *marker)
{
    struct Node *s = &NODES[start];
    VISIT(s->uid, marker);
    Vector2 pos = Vector2Add(s->pos, WIN_CENTER);

    DrawCircleV(pos, R, DARKBLUE);

    for (int i = 0; i < 8; i++)
    {
        if (s->adjs[i] == 0xFFFFFFFF)
            continue;

        if (!STATE(s->adjs[i], marker))
        {
            Vector2 npos = NODES[s->adjs[i]].pos;
            DrawLine(
                pos.x,
                pos.y,
                npos.x + WIN_CENTER.x,
                npos.y + WIN_CENTER.y,
                DARKBLUE
            );
            RenderNodes(s->adjs[i], marker);
        }
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
    float rad = R * 6;

    for (int i = 0; i < 8; i++)
    {
        uint32_t uid = MAP_ORDER++;
        Vector2 pos = {cos(i*PI/4) * rad, sin(i*PI/4) * rad};

        memset(NODES[uid].adjs, 0xFFFFFFFF, 8 * sizeof(uint32_t));

        NODES[uid].uid = uid;
        NODES[uid].pos = pos;
        NODES[uid].adjs[0] = mod(uid - 1, 8) + 1;
        NODES[uid].adjs[1] = mod(uid + 1, 8) + 1;
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
    char *marker = malloc(memsize);
    
    // TraceLog(LOG_TRACE, TextFormat("%zu", memsize));
    // TraceLog(LOG_TRACE, TextFormat("%d", STATE(7, marker)));
    // TraceLog(LOG_TRACE, TextFormat("%d", MAP_ORDER));
    // TraceLog(LOG_TRACE, TextFormat("%zu", memsize));
    // TraceLog(LOG_TRACE, TextFormat("%d", MAP_ORDER / 8));
    // TraceLog(LOG_TRACE, TextFormat("%d", MAP_ORDER % 8));

    while(!WindowShouldClose())
    {
        memset(marker, 0, memsize);

        BeginDrawing();
            ClearBackground(RAYWHITE);
            RenderNodes(start, marker);
        EndDrawing();
    }

    SetTraceLogLevel(LOG_FATAL);
    CloseWindow();
    return 0;
}
