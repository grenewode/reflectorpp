#pragma once

#include <string>
#include <unordered_map>
#include <iostream>

#include "static_utilities.hpp"

/**
 * A property is the core of the reflection module.
 * \param C the class containing this property
 * \param T the type of the member this property wraps.
 * \param member the pointer to the member this property wraps.
 * \param Name a struct which is used to get the name of this property. Should define a static method called get_name().
 */
template <class C, class T, T C::*member, class Name>
class property {
   public:
    using type = T;

    static auto& get_value(C& instance) { return instance.*member; }

    static std::string get_name() { return Name::get_name(); }
};

/**
 * A set of properties contained in a reflectable.
 * \param Pack the set of properties this pack holds.
 */
template <class... Pack>
class property_pack : std::true_type {
   public:
    /**
     * Calls the callback for each property of instance.
     */
    template <class I, class Func>
    static void foreach (Func&& callback, reflect<I> & instance) {
        visit<Func, I, Pack...>(std::forward<Func>(callback), instance);
    }

   private:
    template <class Func, class I, class Last>
    static void visit(Func&& callback, reflect<I>& instance) {
        callback(Last::get_name(), Last::get_value(static_cast<I&>(instance)));
    }

    template <class Func, class I, class H, class H2, class... Tail>
    static void visit(Func&& callback, reflect<I>& instance) {
        callback(H::get_name(), H::get_value(static_cast<I&>(instance)));

        visit<Func, I, H2, Tail...>(std::forward<Func>(callback), instance);
    }

   private:
};

/**
 * Any class using the reflect API must decend from this, and must pass itself into reflect. All members which need to be visible in that class will be defined
 * as:
 * Type PROPERTY(NAME);
 */
template <class Child>
struct reflect {
    using child_t = Child;
    using properties = std::false_type;
    template <class... Ps>
    using expose = property_pack<Ps...>;

    template <class T, T child_t::*member, class Name>
    using property_t = property<child_t, T, member, Name>;

    template <class Func>
    void reflect_foreach(Func&& func) {
        child_t::properties::foreach (std::forward<Func>(func), static_cast<Child&>(*this));
    }
};

#define PROPERTY(name)                                            \
    name;                                                         \
    struct __##name##_name_source {                               \
        static constexpr const char* get_name() { return #name; } \
    };                                                            \
    using name##_property = property_t<decltype(child_t::name), &child_t::name, __##name##_name_source>

/**
 * A utility which uses the curiously recurring template pattern to make defining classes to pass into property lists easier.
 * Subclass must define the following public methods:
 * template <class T>
 * void handle_value(const std::string& name, T& value);
 *
 * AND
 *
 * template <class T>
 * void handle_object(const std::string& name, T& value)
 *
 * See json_interface for some examples
 */
template <class Subclass>
class traverser {
   public:
    template <class T>
    std::enable_if_t<!is_reflection_enabled<T>::value> operator()(const std::string& name, T& value) {
        static_cast<Subclass*>(this)->handle_value(name, value);
    }

    template <class T>
    std::enable_if_t<is_reflection_enabled<T>::value> operator()(const std::string& name, T& value) {
        static_cast<Subclass*>(this)->handle_object(name, value);
    }
};
