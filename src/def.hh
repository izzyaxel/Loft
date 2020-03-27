#pragma once

#include <commons/math/fixed.hh>

#include <memory>

template <typename T> using SP = std::shared_ptr<T>;
template <typename T, typename... Args> SP<T> MS(Args&&... args) {return std::make_shared<T>(std::forward<Args&&>(args)...);}

template <typename T> using UP = std::unique_ptr<T>;
template <typename T, typename... Args> UP<T> MU(Args&&... args) {return std::make_unique<T>(std::forward<Args&&>(args)...);}

template <typename T> using WP = std::weak_ptr<T>;
