#ifndef GAME_DAMAGEFX_H
#define GAME_DAMAGEFX_H

void spawn_floating_damage(Vector2 pos, int damage) {
    for(int i = 0; i < MAX_FLOATING_DAMAGE; i++) {
        if(!floating_damages[i].active) {
            floating_damages[i].position = pos;
            floating_damages[i].damage = damage;
            floating_damages[i].lifetime = 1.0f;
            floating_damages[i].alpha = 1.0f;
            floating_damages[i].active = true;
            break;
        }
    }
}

void spawn_hit_particles(Vector2 pos, Color color, int count) {
    for(int i = 0; i < count; i++) {
        for(int j = 0; j < MAX_HIT_PARTICLES; j++) {
            if(!hit_particles[j].active) {
                hit_particles[j].position = pos;
                float angle = (rand() % 360) * DEG2RAD;
                float speed = 40.0f + (rand() % 60);
                hit_particles[j].velocity = (Vector2){ cosf(angle) * speed, sinf(angle) * speed };
                hit_particles[j].lifetime = 0.4f;
                hit_particles[j].color = color;
                hit_particles[j].active = true;
                break;
            }
        }
    }
}

void update_damage_fx(float dt) {
    for(int i = 0; i < MAX_FLOATING_DAMAGE; i++) {
        if(floating_damages[i].active) {
            floating_damages[i].position.y -= 30.0f * dt;
            floating_damages[i].lifetime -= dt;
            floating_damages[i].alpha = floating_damages[i].lifetime;
            if(floating_damages[i].lifetime <= 0) floating_damages[i].active = false;
        }
    }

    for(int i = 0; i < MAX_HIT_PARTICLES; i++) {
        if(hit_particles[i].active) {
            hit_particles[i].position = Vector2Add(hit_particles[i].position, Vector2Scale(hit_particles[i].velocity, dt));
            hit_particles[i].velocity = Vector2Scale(hit_particles[i].velocity, 0.85f);
            hit_particles[i].lifetime -= dt;
            if(hit_particles[i].lifetime <= 0) hit_particles[i].active = false;
        }
    }
}

void draw_damage_fx(void) {
    for(int i = 0; i < MAX_FLOATING_DAMAGE; i++) {
        if(floating_damages[i].active) {
            Color c = RED;
            c.a = (unsigned char)(floating_damages[i].alpha * 255);
            char text[16];
            sprintf(text, "%d", floating_damages[i].damage);
            DrawText(text, (int)floating_damages[i].position.x, (int)floating_damages[i].position.y, 18, c);
        }
    }

    for(int i = 0; i < MAX_HIT_PARTICLES; i++) {
        if(hit_particles[i].active) {
            float alpha = hit_particles[i].lifetime * 2.5f;
            Color c = Fade(hit_particles[i].color, alpha > 1.0f ? 1.0f : alpha);
            DrawPixelV(hit_particles[i].position, c);
        }
    }
}

#endif // GAME_DAMAGEFX_H
