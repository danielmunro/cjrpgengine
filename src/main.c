#include "header.h"

int showCollisions = 0;

int main(int argc, char *argv[]) {
    int sceneIndex = 0;
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            if (argv[i][1] == 'c') {
                showCollisions = 1;
            } else if (argv[i][1] == 's') {
                sceneIndex = atoi(&argv[i][2]);
            }
        }
    }
    printf("scene %d\n", sceneIndex);
    SetTargetFPS(60);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "cjrpgengine");
    struct Game *g = createGameInstance(sceneIndex, showCollisions);
    Scene *s = getScene(g);
    SetWindowTitle(s->name);
    run(g);
    CloseWindow();
    return 0;
}