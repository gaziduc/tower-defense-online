//
// Created by Gazi on 4/21/2024.
//

#include "Entity.h"

#include "Constants.h"
#include "LogUtils.h"
#include "Player.h"

Entity::Entity(EntityType type, EntityDirection direction, unsigned row_num) {
    unsigned first_anim_index = static_cast<unsigned>(type) * Constants::num_state_per_entity;
    for (unsigned i = first_anim_index; i < first_anim_index + Constants::num_state_per_entity; i++) {
        _animations.push_back(Constants::get_animation(static_cast<Constants::Anim>(i)));
    }

    _entity_type = type;
    _entity_direction = direction;
    _entity_state = EntityState::RUNNING;
    _animation_frame = 0;
    _row_num = row_num;

    int w = 0;
    int h = 0;
    SDL_QueryTexture(get_entity_texture(), nullptr, nullptr, &w, &h);
    float height_ratio = static_cast<float>(h) / Constants::entity_height;
    float final_width = static_cast<float>(w) / height_ratio;
    _dst_pos.x = direction == EntityDirection::LEFT_TO_RIGHT ? -final_width : 1920;
    _dst_pos.y = static_cast<float>(row_num + 0.5) * Constants::row_height - Constants::entity_height / 2;
    _dst_pos.w = final_width;
    _dst_pos.h = Constants::entity_height;

    _max_health = 100;
    _health = _max_health;
}

void Entity::set_entity_state(EntityState state) {
    _entity_state = state;
    _animation_frame = 0;

    int w = 0;
    int h = 0;
    SDL_QueryTexture(get_entity_texture(), nullptr, nullptr, &w, &h);
    float height_ratio = static_cast<float>(h) / Constants::entity_height;
    float final_width = static_cast<float>(w) / height_ratio;
    _dst_pos.y = static_cast<float>(_row_num + 0.5) * Constants::row_height - Constants::entity_height / 2;
    _dst_pos.w = final_width;
    _dst_pos.h = Constants::entity_height;
}

SDL_Texture* Entity::get_entity_texture() {
    return _animations[_entity_state]->get_texture(_animation_frame);
}

SDL_FRect* Entity::get_entity_dst_pos() {
    return &_dst_pos;
}

void Entity::move() {
    if (_entity_state != EntityState::RUNNING) {
        set_entity_state(EntityState::RUNNING);
    } else {
        _animation_frame++;
        reset_animation_ifn();
    }

    _dst_pos.x += _entity_direction == EntityDirection::LEFT_TO_RIGHT ? 1.5 : -1.5;
}

void Entity::set_state(EntityState state) {
    if (_entity_state != state) {
        set_entity_state(state);
    } else {
        _animation_frame++;
        reset_animation_ifn();
    }
}

void Entity::attack(Entity& enemy) {
    set_state(EntityState::ATTACKING);

    if (_animation_frame == 0) {
        enemy.decrease_health(get_attack_damage());
    }
}

void Entity::attack(Player& enemy) {
    set_state(EntityState::ATTACKING);

    if (_animation_frame == 0) {
        enemy.decrease_health(get_attack_damage());
    }
}

void Entity::reset_animation_ifn() {
    if (_animations[_entity_state]->get_num_frames() <= _animation_frame) {
        _animation_frame = 0;
    }
}


Entity::EntityDirection Entity::get_entity_direction() {
    return _entity_direction;
}


SDL_RendererFlip Entity::get_render_flip_from_direction() {
    return _entity_direction == EntityDirection::LEFT_TO_RIGHT ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
}

float Entity::get_range() {
    switch (_entity_type) {
        case EntityType::GUN_MAN:
            return 220;
        case EntityType::SWORD_MAN:
            return 0;
        default:
            LogUtils::log_message(LogUtils::ERROR, "Unkwown entity type: " + _entity_type);
            return 0;
    }
}

void Entity::decrease_health(int health_to_decrease) {
    _health -= health_to_decrease;
}

int Entity::get_attack_damage() {
    switch (_entity_type) {
        case EntityType::GUN_MAN:
            return 10;
        case EntityType::SWORD_MAN:
            return 40;
        default:
            LogUtils::log_message(LogUtils::ERROR, "Unkwown entity type: " + _entity_type);
            return 0;
    }
}

bool Entity::is_dead() {
    return _health <= 0;
}

int Entity::get_cost() {
    switch (_entity_type) {
        case SWORD_MAN:
            return 40;
        case GUN_MAN:
            return 30;
        default:
            LogUtils::log_message(LogUtils::ERROR, "Unknown entity type: " + _entity_type);
            return 0;
    }
}



void Entity::render_health_bar(SDL_Renderer *renderer) {
    SDL_FRect dst_rect = {
        .x = _dst_pos.x + _dst_pos.w / 2 - Constants::health_bar_width / 2,
        .y = _dst_pos.y - Constants::health_bar_height,
        .w = Constants::health_bar_width,
        .h = Constants::health_bar_height
    };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 192);
    SDL_RenderFillRectF(renderer, &dst_rect);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 192);
    dst_rect.w = dst_rect.w * static_cast<float>(_health) / static_cast<float>(_max_health);
    SDL_RenderFillRectF(renderer, &dst_rect);
}



