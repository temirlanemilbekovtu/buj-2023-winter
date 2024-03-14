#ifndef BUJ_2024_WINTER_PLAYER_H
#define BUJ_2024_WINTER_PLAYER_H

#include <godot_cpp/classes/character_body2d.hpp>
#include <godot_cpp/classes/input_event_key.hpp>
#include <godot_cpp/classes/input_event.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/timer.hpp>
#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/font.hpp>

namespace godot {
    class Player : public CharacterBody2D {
        GDCLASS(Player, CharacterBody2D)

    private:
        float _speed;
        float _max_velocity_y;

        float _jump_height;
        float _jump_time_to_peak;
        float _jump_time_to_descent;
        float _jump_buffer_time;
        float _jump_coyote_time;
        float _jump_gravity;
        float _fall_gravity;

        float _interval_since_jump_pressed;
        float _interval_since_on_floor;
        float _default_delta;
        int _direction;

        void _try_jump();
        void _move();
        void _flip();

    protected:
        static void _bind_methods();

    public:
        Player();
        ~Player();

        void _input(const Ref <godot::InputEvent> &event) override;

        void _ready() override;
        void _physics_process(double delta) override;

        float get_speed() const;
        void set_speed(float speed);
        float get_max_velocity_y() const;
        void set_max_velocity_y(float velocity);
        float get_jump_height() const;
        void set_jump_height(float height);
        float get_jump_time_to_peak() const;
        void set_jump_time_to_peak(float time);
        float get_jump_time_to_descent() const;
        void set_jump_time_to_descent(float time);
        float get_jump_buffer_time() const;
        void set_jump_buffer_time(float time);
        float get_jump_coyote_time() const;
        void set_jump_coyote_time(float time);

        float get_interval_since_jump_pressed() const;
        float get_interval_since_on_floor() const;
        float get_gravity() const;
    };

}


#endif //BUJ_2024_WINTER_PLAYER_H
