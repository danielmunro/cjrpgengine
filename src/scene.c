typedef struct Scene {
    Sprite *sprites[MAX_SPRITES];
    Player *player;
    Tilemap *tilemap;
    int layers;
    int type;
    char name[SCENE_NAME_MAX_LENGTH];
    int showCollisions;
} Scene;

typedef struct SceneType {
    int code;
    const char *scene;
} SceneType;

const SceneType sceneTypes[] = {
    {SCENE_TYPE_FREE_MOVE, "free-move"},
    {SCENE_TYPE_FIGHT, "fight"},
    {SCENE_TYPE_MENU, "menu"},
};

Object *getObject(Scene *s, int index) {
    for (int l = 0; l < s->layers; l++) {
        int o = 0;
        while (s->tilemap->objects[o] != NULL) {
            if (s->tilemap->objects[o]->tile == index) {
                return s->tilemap->objects[o];
            }
            o++;
        }
    }
    return NULL;
}

Vector2D getTileCount(Scene *s) {
    int x = SCREEN_WIDTH / s->tilemap->size.x + 1;
    int y = SCREEN_HEIGHT / s->tilemap->size.y + 2;
    return (Vector2D){x, y};
}

Vector2D getStartTileCoords(Vector2 playerPos, Vector2D tiles) {
    return (Vector2D){
            (int)playerPos.x - (tiles.x / 2),
            (int)playerPos.y - (tiles.y / 2),
    };
}

int checkCollision(Scene *s, Vector2 playerPos) {
    Vector2D tiles = getTileCount(s);
    Vector2D start = getStartTileCoords(playerPos, tiles);
    Vector2D sz = s->tilemap->size;
    int i = 0;
    while (i < MAX_LAYER_COUNT && i < s->layers) {
        for (int y = -1; y < tiles.y; y++) {
            for (int x = -1; x < tiles.x; x++) {
                if (start.x + x < 0 || start.y + y < 0 || start.x + x > MAX_LAYER_SIZE ||
                    start.y + y > MAX_LAYER_SIZE) {
                    continue;
                }
                int index = s->tilemap->layers[i][start.y + y][start.x + x];
                if (index <= 0) {
                    continue;
                }
                int xint = (int)playerPos.x;
                float xdiff = playerPos.x - (float)xint;
                int yint = (int)playerPos.y;
                float ydiff = playerPos.y - (float)yint;
                Object *o = getObject(s, index - 1);
                if (o != NULL) {
                    Rectangle r = {
                            (float) (sz.x * x) - (xdiff * (float) sz.x) + o->rect.x,
                            (float) (sz.y * y) - (ydiff * (float) sz.y) + o->rect.y,
                            o->rect.width,
                            o->rect.height,
                    };
                    Rectangle pr = {
                            s->player->sprite->position.x + 3,
                            s->player->sprite->position.y + 12,
                            HUMANOID_WIDTH - 3,
                            HUMANOID_HEIGHT - 12,
                    };
                    if (CheckCollisionRecs(pr, r)) {
                        return 1;
                    }
                }
            }
        }
        i++;
    }
    return 0;
}

int checkMoveKey(Scene *s, int key, int direction, Vector2 pos) {
    if (IsKeyDown(key)) {
        if (checkCollision(s, pos)) {
            return 0;
        }
        movePlayer(s->player, direction, pos);
        return 1;
    }
    return 0;
}

void checkInput(Scene *s) {
    if (s->type == SCENE_TYPE_FREE_MOVE) {
        int moveR = checkMoveKey(
                s,
                KEY_RIGHT,
                DIRECTION_RIGHT,
                (Vector2){s->player->pos.x + MOVE_AMOUNT, s->player->pos.y}
        );
        int moveL = checkMoveKey(
                s,
                KEY_LEFT,
                DIRECTION_LEFT,
                (Vector2){s->player->pos.x - MOVE_AMOUNT, s->player->pos.y}
        );
        int moveU = checkMoveKey(
                s,
                KEY_UP,
                DIRECTION_UP,
                (Vector2){s->player->pos.x, s->player->pos.y - MOVE_AMOUNT}
        );
        int moveD = checkMoveKey(
                s,
                KEY_DOWN,
                DIRECTION_DOWN,
                (Vector2){s->player->pos.x, s->player->pos.y + MOVE_AMOUNT}
        );
        s->player->isMoving = moveR || moveL || moveU || moveD;
    }
}

void drawLayers(Scene *s) {
    Vector2D tiles = getTileCount(s);
    Vector2D start = getStartTileCoords(s->player->pos, tiles);
    Vector2D sz = s->tilemap->size;
    int i = 0;
    while (i < MAX_LAYER_COUNT && i < s->layers) {
        for (int y = -1; y < tiles.y; y++) {
            for (int x = -1; x < tiles.x; x++) {
                if (start.x + x < 0 ||
                    start.y + y < 0 ||
                    start.x + x > MAX_LAYER_SIZE ||
                    start.y + y > MAX_LAYER_SIZE) {
                    continue;
                }
                int index = s->tilemap->layers[i][start.y + y][start.x + x];
                if (index <= 0) {
                    continue;
                }
                Vector2D t = getTileFromIndex(s->tilemap, index);
                Rectangle frameRec = {
                        (float) (t.x * sz.x),
                        (float) (t.y * sz.y),
                        (float) sz.x,
                        (float) sz.y
                };
                int xint = (int)s->player->pos.x;
                float xdiff = s->player->pos.x - (float)xint;
                int yint = (int)s->player->pos.y;
                float ydiff = s->player->pos.y - (float)yint;
                Vector2 pos = {
                        (float) (sz.x * x) - (xdiff * (float)sz.x),
                        (float) (sz.y * y) - (ydiff * (float)sz.y)
                };
                DrawTextureRec(
                        s->tilemap->source,
                        frameRec,
                        pos,
                        WHITE
                );
                if (s->showCollisions) {
                    Object *o = getObject(s, index - 1);
                    if (o != NULL) {
                        Rectangle r = {
                                (float) (sz.x * x) - (xdiff * (float) sz.x) + o->rect.x,
                                (float) (sz.y * y) - (ydiff * (float) sz.y) + o->rect.y,
                                o->rect.width,
                                o->rect.height,
                        };
                        Color collision = PINK;
                        Rectangle pr = {
                                s->player->sprite->position.x,
                                s->player->sprite->position.y,
                                HUMANOID_WIDTH,
                                HUMANOID_HEIGHT,
                        };
                        if (CheckCollisionRecs(pr, r)) {
                            collision = PURPLE;
                        }
                        DrawRectangleRec(r, collision);
                    }
                }
            }
        }
        i++;
    }
}

void drawScene(Scene *s) {
    ClearBackground(RAYWHITE);
    drawLayers(s);
    drawSprite(s->player->sprite);
}
