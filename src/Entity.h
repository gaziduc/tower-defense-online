//
// Created by Gazi on 4/21/2024.
//

#ifndef ENTITY_H
#define ENTITY_H

#include <memory>
#include <optional>
#include <SDL_net.h>

#include "Animation.h"


class Player;

class Entity {
public:
    enum EntityType {
        SWORD_MAN = 0,
        GUN_MAN,

        NUM_ENTITY_TYPE
    };

    enum EntityState {
        RUNNING = 0,
        ATTACKING,
        HURT,
        IDLE
    };

    enum EntityDirection {
        UNDEFINED = -1,
        LEFT_TO_RIGHT = 0,
        RIGHT_TO_LEFT,
        NUM_DIRECTIONS
    };

    Entity(EntityType type, EntityDirection direction, unsigned row_num, Uint32 entity_id);
    EntityType get_entity_type();
    int get_row_num();
    void set_entity_state(EntityState state, bool is_init);
    SDL_Texture* get_entity_texture();
    SDL_FRect* get_entity_dst_pos();
    void set_pos_x(float pos_x);
    void move(int num_times = 1);
    void attack(Entity& enemy);
    void attack(Player& enemy);
    void reset_animation_ifn();
    EntityDirection get_entity_direction();
    SDL_RendererFlip get_render_flip_from_direction();
    float get_range();
    void decrease_health(int health_to_decrease);
    int get_attack_damage();
    bool is_dead();
    static int get_cost(EntityType entity_type);
    bool can_attack_entity(std::optional<Entity>& enemy_entity_first);
    bool can_attack_player(std::optional<Entity>& enemy_entity_first);
    bool is_beyond_entity(Entity& first_entity);
    int get_health();
    int get_max_health();
    unsigned long get_id();
    void set_health(int health);
    void set_max_health(int max_health);
    EntityState get_state();

    // Rendering
    void render_health_bar(SDL_Renderer *renderer, float ratio_x, float ratio_y);

    // Network
    void send_death_to_client(std::vector<std::pair<TCPsocket, Player>>& sockets);

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

    unsigned long _id;
    bool _is_server_side;

    void set_state(EntityState state);
};



#endif //ENTITY_H
