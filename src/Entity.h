//
// Created by Gazi on 4/21/2024.
//

#ifndef ENTITY_H
#define ENTITY_H

#include <memory>

#include "Animation.h"


class Entity {
public:
    enum EntityType {
        SWORD_MAN = 0,
        GUN_MAN
    };

    enum EntityState {
        RUNNING = 0,
        ATTACKING,
        HURT
    };

    enum EntityDirection {
        LEFT_TO_RIGHT = 0,
        RIGHT_TO_LEFT,
        NUM_DIRECTIONS
    };

    Entity(EntityType type, EntityDirection direction, unsigned row_num);
    void set_entity_state(EntityState state);
    SDL_Texture* get_entity_texture();
    SDL_FRect* get_entity_dst_pos();
    void move();
    void attack(Entity& enemy);
    void reset_animation_ifn();
    EntityDirection get_entity_direction();
    SDL_RendererFlip get_render_flip_from_direction();
    float get_range();
    void decrease_health(int health_to_decrease);
    int get_attack_damage();
    bool is_dead();

    // Rendering
    void render_health_bar(SDL_Renderer *renderer);

private:
    EntityType _entity_type;
    EntityState _entity_state;
    EntityDirection _entity_direction;
    SDL_FRect _dst_pos;
    unsigned _animation_frame;
    std::vector<std::shared_ptr<Animation>> _animations;
    unsigned _row_num;
    int _max_health;
    int _health;
};



#endif //ENTITY_H
