#include "player.h"

#include <algorithm>
#include <cmath>

#include <godot_cpp/core/class_db.hpp>

using namespace std;
using namespace godot;

void Player::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_speed"), &Player::get_speed);
    ClassDB::bind_method(D_METHOD("set_speed", "speed"), &Player::set_speed);
    ClassDB::add_property("Player", PropertyInfo(Variant::FLOAT, "speed"),
                          "set_speed", "get_speed");
    ClassDB::bind_method(D_METHOD("get_max_velocity_y"), &Player::get_max_velocity_y);
    ClassDB::bind_method(D_METHOD("set_max_velocity_y", "velocity"), &Player::set_max_velocity_y);
    ClassDB::add_property("Player", PropertyInfo(Variant::FLOAT, "max_velocity_y"),
                          "set_max_velocity_y", "get_max_velocity_y");
    ClassDB::bind_method(D_METHOD("get_jump_height"), &Player::get_jump_height);
    ClassDB::bind_method(D_METHOD("set_jump_height", "height"), &Player::set_jump_height);
    ClassDB::add_property("Player", PropertyInfo(Variant::FLOAT, "jump_height"),
                          "set_jump_height", "get_jump_height");
    ClassDB::bind_method(D_METHOD("get_jump_time_to_peak"), &Player::get_jump_time_to_peak);
    ClassDB::bind_method(D_METHOD("set_jump_time_to_peak", "time"), &Player::set_jump_time_to_peak);
    ClassDB::add_property("Player", PropertyInfo(Variant::FLOAT, "jump_time_to_peak"),
                          "set_jump_time_to_peak", "get_jump_time_to_peak");
    ClassDB::bind_method(D_METHOD("get_jump_time_to_descent"), &Player::get_jump_time_to_descent);
    ClassDB::bind_method(D_METHOD("set_jump_time_to_descent", "time"), &Player::set_jump_time_to_descent);
    ClassDB::add_property("Player", PropertyInfo(Variant::FLOAT, "jump_time_to_descent"),
                          "set_jump_time_to_descent", "get_jump_time_to_descent");
    ClassDB::bind_method(D_METHOD("get_jump_buffer_time"), &Player::get_jump_buffer_time);
    ClassDB::bind_method(D_METHOD("set_jump_buffer_time", "time"), &Player::set_jump_buffer_time);
    ClassDB::add_property("Player", PropertyInfo(Variant::FLOAT, "jump_buffer_time"),
                          "set_jump_buffer_time", "get_jump_buffer_time");
    ClassDB::bind_method(D_METHOD("get_jump_coyote_time"), &Player::get_jump_coyote_time);
    ClassDB::bind_method(D_METHOD("set_jump_coyote_time", "time"), &Player::set_jump_coyote_time);
    ClassDB::add_property("Player", PropertyInfo(Variant::FLOAT, "jump_coyote_time"),
                          "set_jump_coyote_time", "get_jump_coyote_time");
    ClassDB::bind_method(D_METHOD("get_interval_since_jump_pressed"), &Player::get_interval_since_jump_pressed);
    ClassDB::bind_method(D_METHOD("get_interval_since_on_floor"), &Player::get_interval_since_on_floor);
    ClassDB::bind_method(D_METHOD("get_gravity"), &Player::get_gravity);
}

Player::Player() {
    if (Engine::get_singleton()->is_editor_hint())
        set_process_mode(ProcessMode::PROCESS_MODE_DISABLED);

    _speed = 10.0;
    _max_velocity_y = 1000.0;

    _jump_height = 49.0;
    _jump_time_to_peak = 1.0;
    _jump_time_to_descent = 1.0;
    _jump_buffer_time = 0.1;
    _jump_coyote_time = 0.1;

    _interval_since_jump_pressed = 0.0;
    _interval_since_on_floor = 0.0;
    _direction = 0;
}

Player::~Player() {}

void Player::_input(const Ref <godot::InputEvent> &event) {
    InputEventKey* event_key = dynamic_cast<InputEventKey*>(event.ptr());
    if (event->is_echo() || !event_key)
        return;

    Key keycode = event_key->get_keycode();

    switch (keycode) {
        case KEY_A:
        case KEY_D: {
            int input_dir = keycode == KEY_D ? 1 : -1;
            if (event_key->is_pressed())
                _direction = input_dir;
            else if (_direction == input_dir)
                _direction = 0;
            break;
        }
        case KEY_SPACE:
            if (event_key->is_pressed())
                _try_jump();
                _interval_since_jump_pressed = 0.0;
            break;
        default:
            break;
    }
}

void Player::_ready() {
    _jump_gravity = 2.0 * _jump_height / powf(_jump_time_to_peak, 2.0);
    _fall_gravity = 2.0 * _jump_height / powf(_jump_time_to_descent, 2.0);

    _default_delta = 1.0 / Engine::get_singleton()->get_physics_ticks_per_second();
}

void Player::_physics_process(double delta) {
    _move();

    if (is_on_floor()) {
        if (_interval_since_jump_pressed < _jump_buffer_time)
            _try_jump();
        _interval_since_on_floor = 0.0;
    }
    else {
        _interval_since_on_floor += delta;
        _interval_since_jump_pressed += delta;
    }
}


void Player::_try_jump() {
    if (is_on_floor() || _interval_since_on_floor < _jump_coyote_time) {
        auto velocity = Vector2(get_velocity().x,
                                -1.0 * _jump_gravity * _jump_time_to_peak);
        set_velocity(velocity);
    }
}

void Player::_move() {
    float delta = get_physics_process_delta_time();

    if (_direction != get_scale().x && _direction != 0)
        _flip();

    auto velocity = Vector2(_speed * _direction * delta / _default_delta,
                            (get_velocity().y + get_gravity() * delta) * delta / _default_delta);
    velocity.y = clamp(-_max_velocity_y, _max_velocity_y, velocity.y);

    set_velocity(velocity);
    move_and_slide();
}

void Player::_flip() {
    auto new_scale = get_scale();
    new_scale.x *= -1;
    set_scale(new_scale);
}

#pragma region getset

float Player::get_speed() const { return _speed; }
void Player::set_speed(float speed) {
    if (speed >= 0)
        _speed = speed;
}

float Player::get_max_velocity_y() const { return _max_velocity_y; }
void Player::set_max_velocity_y(float velocity) {
    if (velocity >= 0)
        _max_velocity_y = velocity;
}

float Player::get_jump_height() const { return _jump_height; }
void Player::set_jump_height(float height) {
    if (height >= 0)
        _jump_height = height;
}

float Player::get_jump_time_to_peak() const { return _jump_time_to_peak; }
void Player::set_jump_time_to_peak(float time) {
    if (time >= 0)
        _jump_time_to_peak = time;
}

float Player::get_jump_time_to_descent() const { return _jump_time_to_descent; }
void Player::set_jump_time_to_descent(float time) {
    if (time >= 0)
        _jump_time_to_descent = time;
}

float Player::get_jump_buffer_time() const { return _jump_buffer_time; }
void Player::set_jump_buffer_time(float time) {
    if (time >= 0)
        _jump_buffer_time = time;
}

float Player::get_jump_coyote_time() const { return _jump_coyote_time; }
void Player::set_jump_coyote_time(float time) {
    if (time >= 0)
        _jump_coyote_time = time;
}

float Player::get_interval_since_jump_pressed() const { return _interval_since_jump_pressed; }

float Player::get_interval_since_on_floor() const { return _interval_since_on_floor; }

float Player::get_gravity() const {
    return get_velocity().y < 0.0 ? _jump_gravity : _fall_gravity;
}

#pragma endregion getset