#pragma once

#include "reflection.hpp"
#include "json.hpp"

class to_json : public traverser<to_json> {
   public:
    to_json(nlohmann::json& root) : root(root) {}

    template <class T>
    void handle_value(const std::string& name, T& value) {
        (*current_root)[name] = value;
    }

    template <class T>
    void handle_object(const std::string& name, T& value) {
        auto old_root = current_root;
        current_root = &root[name];
        value.reflect_foreach(*this);
        current_root = old_root;
    }

   private:
    nlohmann::json& root;
    nlohmann::json* current_root = &root;
};

struct from_json : public traverser<from_json> {
    from_json(nlohmann::json& root) : root(root) {}

    template <class T>
    void handle_value(const std::string& name, T& value) {
        value = (*current_root)[name].get<T>();
    }

    template <class T>
    void handle_object(const std::string& name, T& value) {
        auto old_root = current_root;
        current_root = &root[name];
        value.reflect_foreach(*this);
        current_root = old_root;
    }

   private:
    nlohmann::json& root;
    nlohmann::json* current_root = &root;
};

struct json_driver {
    template <class T>
    nlohmann::json operator()(T& reflectable) {
        static_assert(is_reflection_enabled<T>::value, "Drivers only work with reflectable types");

        nlohmann::json document;
        reflectable.reflect_foreach(to_json(document));

        return document;
    }

    template <class T>
    nlohmann::json operator()(T& reflectable, nlohmann::json document) {
        static_assert(is_reflection_enabled<T>::value, "Drivers only work with reflectable types");

        reflectable.reflect_foreach(from_json(document));

        return document;
    }
};
