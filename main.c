#include <raylib.h>
#include <stdint.h>
#include <string.h>

#define BTH_SALLOC_IMPLEMENTATION
#include "bth_salloc.h"

#define WIN_WIDTH  800
#define WIN_HEIGHT 600

#define GET_IDX(s) ((s)/8)
#define GET_MASK(s) ((s)%8)
#define VISIT(n, m) (m[GET_IDX(n)] |= (0x01 << GET_MASK(n)))
#define STATE(n, m) (m[GET_IDX(n)] & (0x01 << GET_MASK(n)))
#define R 20.0

static uint32_t MAP_ORDER = 0;

struct Node
{
    uint32_t uid;
    Vector2 pos;
    char units;
    char count;
    struct Node *adjs; // max 8 neighbors
};

typedef struct Node Node;

void RenderNodes(Node *start, char *marker)
{
    VISIT(start->uid, marker);

    Vector2 pos = {
        WIN_WIDTH / 2.0 + start->pos.x,
        WIN_HEIGHT / 2.0 + start->pos.y,
    };

    DrawPoly(pos, 8, R, 0, DARKBLUE);

    // TraceLog(LOG_TRACE, TextFormat("%d", start->uid));

    for (char i = 0; i < start->count; i++)
    {
        uint32_t id = start->adjs[i].uid;
        if (!STATE(id, marker))
        {
            RenderNodes(&start->adjs[i], marker);
        }
    }
}

void InitMap(Node **start)
{
    Node *s = smalloc(sizeof(Node));

    s->uid = MAP_ORDER++;
    s->pos = (Vector2){0.0, 0.0};
    s->units = 0;
    s->count = 8;
    s->adjs = smalloc(8 * sizeof(Node));
    memset(s->adjs, 0, 8 * sizeof(Node));

    s->adjs[0].uid = MAP_ORDER++;
    s->adjs[1].uid = MAP_ORDER++;
    s->adjs[2].uid = MAP_ORDER++;
    s->adjs[3].uid = MAP_ORDER++;
    s->adjs[4].uid = MAP_ORDER++;
    s->adjs[5].uid = MAP_ORDER++;
    s->adjs[7].uid = MAP_ORDER++;
    s->adjs[8].uid = MAP_ORDER++;

    float p = R * 4;
    s->adjs[0].pos = (Vector2){0, p};
    s->adjs[1].pos = (Vector2){p, p};
    s->adjs[2].pos = (Vector2){p, 0};
    s->adjs[3].pos = (Vector2){p, -p};
    s->adjs[4].pos = (Vector2){-p, 0};
    s->adjs[5].pos = (Vector2){-p, -p};
    s->adjs[6].pos = (Vector2){-p, 0};
    s->adjs[7].pos = (Vector2){-p, p};

    *start = s;
}

int main(void)
{
    SetTraceLogLevel(LOG_FATAL);
    InitWindow(WIN_WIDTH, WIN_HEIGHT, "Bob's R.I.S.K");
    // SetTraceLogLevel(LOG_TRACE);

    Node *start;
    InitMap(&start);
    size_t memsize = MAP_ORDER / 8 + (MAP_ORDER % 8) == 0 ? 0 : 1;
    char *marker = malloc(memsize);
    
    // TraceLog(LOG_TRACE, TextFormat("%zu", memsize));
    // TraceLog(LOG_TRACE, TextFormat("%d", STATE(7, marker)));

    while(!WindowShouldClose())
    {
        memset(marker, 0, memsize);

        BeginDrawing();
            ClearBackground(RAYWHITE);
            RenderNodes(start, marker);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
