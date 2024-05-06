//
// Created by Gazi on 5/4/2024.
//

#include "Game.h"

#include <vector>

#include "../Constants.h"
#include "../Entity.h"
#include "../Player.h"


void Game::process_game(Player& player1, Player& player2, bool is_server_side, std::vector<std::pair<TCPsocket, Player>>& sockets) {
    for (int row_index = 0; row_index < Constants::NUM_BATTLE_ROWS; row_index++) {
        std::vector<Entity>& player1_entity_list = player1.get_entities_map()[row_index];
        std::vector<Entity>& player2_entity_list = player2.get_entities_map()[row_index];

        std::optional<Entity> player_entity_first = std::nullopt;
        int player_entity_first_index = -1;
        if (!player1_entity_list.empty()) {
            player_entity_first = player1_entity_list[0];
            player_entity_first_index = 0;
            for (int i = 1; i < player1_entity_list.size(); i++) {
                Entity current_entity = player1_entity_list[i];
                if (current_entity.is_beyond_entity(*player_entity_first)) {
                    player_entity_first = current_entity;
                    player_entity_first_index = i;
                }
            }
        }

        std::optional<Entity> enemy_entity_first = std::nullopt;
        int enemy_entity_first_index = -1;
        if (!player2_entity_list.empty()) {
            enemy_entity_first = player2_entity_list[0];
            enemy_entity_first_index = 0;
            for (int i = 1; i < player2_entity_list.size(); i++) {
                Entity current_entity = player2_entity_list[i];
                if (current_entity.is_beyond_entity(*enemy_entity_first)) {
                    enemy_entity_first = current_entity;
                    enemy_entity_first_index = i;
                }
            }
        }


        for (Entity& player_entity : player1_entity_list) {
            if (player_entity.can_attack_entity(enemy_entity_first)) {
                player_entity.attack(player2_entity_list[enemy_entity_first_index]);
            } else if (player_entity.can_attack_player(enemy_entity_first)) {
                player_entity.attack(player2);
            } else {
                player_entity.move();
            }
        }


        for (Entity& enemy_entity: player2_entity_list) {
            if (enemy_entity.can_attack_entity(player_entity_first)) {
                enemy_entity.attack(player1_entity_list[player_entity_first_index]);
            } else if (enemy_entity.can_attack_player(player_entity_first)) {
                enemy_entity.attack(player1);
            } else {
                enemy_entity.move();
            }
        }

        std::vector<Entity>::iterator entity_it = player1_entity_list.begin();
        while (entity_it != player1_entity_list.end()) {
            if (entity_it->is_dead()) {
                player2.increase_money(Entity::get_cost(entity_it->get_entity_type()));
                if (is_server_side) {
                    entity_it->send_death_to_client(sockets);
                }
                entity_it = player1_entity_list.erase(entity_it);
            } else {
                entity_it++;
            }
        }

        entity_it = player2_entity_list.begin();
        while (entity_it != player2_entity_list.end()) {
            if (entity_it->is_dead()) {
                player1.increase_money(Entity::get_cost(entity_it->get_entity_type()));
                if (is_server_side) {
                    entity_it->send_death_to_client(sockets);
                }
                entity_it = player2_entity_list.erase(entity_it);
            } else {
                entity_it++;
            }
        }
    }
}
