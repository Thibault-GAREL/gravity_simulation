#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <math.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define NUM_OBJECTS 100
#define GRAVITY_CONSTANT 1
#define RAYON 5
#define FROTTEMENT 0.99

typedef struct {
    float x, y;
    float vx, vy;
} Object;

void checkBoundaryCollision(Object *circle) {
    if (circle->x - RAYON < 0 || circle->x + RAYON > SCREEN_WIDTH) {
        circle->vx *= -1;  // Inversion de la vitesse horizontale
    }
    if (circle->y - RAYON < 0 || circle->y + RAYON > SCREEN_HEIGHT) {
        circle->vy *= -1;  // Inversion de la vitesse verticale
    }
}

void checkCircleCollision(Object *obj1, Object *obj2) {
    float dx = obj2->x - obj1->x;
    float dy = obj2->y - obj1->y;
    float distance = sqrt(dx * dx + dy * dy);

    if (distance <= 2 * RAYON) {
        float overlap = 2 * RAYON - distance;
        obj1->x -= overlap / 2 * (dx / distance);
        obj1->y -= overlap / 2 * (dy / distance);
        obj2->x += overlap / 2 * (dx / distance);
        obj2->y += overlap / 2 * (dy / distance);

        // Calcul de la normale et de la tangente à la collision
        float normal_x = dx / distance;
        float normal_y = dy / distance;
        float tangent_x = -normal_y;
        float tangent_y = normal_x;

        // Projection des vitesses des cercles sur la normale et la tangente
        float velocity1_normal = obj1->vx * normal_x + obj1->vy * normal_y;
        float velocity1_tangent = obj1->vx * tangent_x + obj1->vy * tangent_y;
        float velocity2_normal = obj2->vx * normal_x + obj2->vy * normal_y;
        float velocity2_tangent = obj2->vx * tangent_x + obj2->vy * tangent_y;

        // Calcul des nouvelles vitesses après la collision (conservation de la quantité de mouvement)
        float new_velocity1_normal = velocity2_normal;
        float new_velocity2_normal = velocity1_normal;

        // Mise à jour des vitesses des cercles après la collision
        obj1->vx = new_velocity1_normal * normal_x + velocity1_tangent * tangent_x;
        obj1->vy = new_velocity1_normal * normal_y + velocity1_tangent * tangent_y;
        obj2->vx = new_velocity2_normal * normal_x + velocity2_tangent * tangent_x;
        obj2->vy = new_velocity2_normal * normal_y + velocity2_tangent * tangent_y;
    }
}

void applyGravity(Object *obj1, Object *obj2) {
    float dx = obj2->x - obj1->x;
    float dy = obj2->y - obj1->y;
    float distance = sqrt(dx * dx + dy * dy);
    float force = GRAVITY_CONSTANT / (distance * distance);

    float angle = atan2(dy, dx);                //calcule de l'angle en radiant
    float fx = force * cos(angle);
    float fy = force * sin(angle);

//    float random = rand()%2;

    if (distance <= (RAYON*2)){

        checkCircleCollision(obj1, obj2);

        /*obj1->vx = -obj1->vx;
        obj1->vy = -obj1->vy;

        obj2->vx = -obj2->vx;
        obj2->vy = -obj2->vy;

        obj1->vx -= fx;
        obj1->vy -= fy;

        obj2->vx += fx;
        obj2->vy += fy;*/
    }
    else {
        obj1->vx += fx;
        obj1->vy += fy;

        obj2->vx -= fx;
        obj2->vy -= fy;
    }
}

int main() {
    ALLEGRO_DISPLAY *display = NULL;
    ALLEGRO_EVENT_QUEUE *event_queue = NULL;
    ALLEGRO_TIMER *timer = NULL;
    bool running = true;
    Object objects[NUM_OBJECTS];

    if (!al_init()) {
        fprintf(stderr, "Failed to initialize Allegro!\n");
        return -1;
    }

    display = al_create_display(SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!display) {
        fprintf(stderr, "Failed to create display!\n");
        return -1;
    }

    al_install_keyboard();
    al_init_primitives_addon();

    event_queue = al_create_event_queue();
    timer = al_create_timer(1.0 / 60);

    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));

    for (int i = 0; i < NUM_OBJECTS; i++) {
        objects[i].x = rand() % SCREEN_WIDTH;
        objects[i].y = rand() % SCREEN_HEIGHT;
        objects[i].vx = 0;
        objects[i].vy = 0;
    }

    al_start_timer(timer);

    while (running) {
        ALLEGRO_EVENT event;
        al_wait_for_event(event_queue, &event);

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            running = false;
        } else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                running = false;
            }
        } else if (event.type == ALLEGRO_EVENT_TIMER) {
            for (int i = 0; i < NUM_OBJECTS; i++) {
                Object *obj1 = &objects[i];

                for (int j = i + 1; j < NUM_OBJECTS; j++) {
                    Object *obj2 = &objects[j];

                    applyGravity(obj1, obj2);
                    checkBoundaryCollision(obj1);
                    checkBoundaryCollision(obj2);
                }

                obj1->x += obj1->vx;
                obj1->y += obj1->vy;
                obj1->vx *= FROTTEMENT; // Damping factor to simulate friction
                obj1->vy *= FROTTEMENT; // Damping factor to simulate friction
            }

            al_clear_to_color(al_map_rgb(40, 40, 60));

for (int i = 0; i < NUM_OBJECTS; i++) {
Object *obj = &objects[i];

al_draw_circle(obj->x, obj->y, RAYON, al_map_rgb(255, 255, 255), 2);  //sqrtf(powf(obj->vx, 2) + powf(obj->vy, 2))*30, sqrtf(powf(obj->vx, 2) + powf(obj->vy, 2))*30, sqrtf(powf(obj->vx, 2) + powf(obj->vy, 2))*30
}

al_flip_display();
}
}

al_destroy_display(display);
al_destroy_event_queue(event_queue);
al_destroy_timer(timer);

return 0;
}