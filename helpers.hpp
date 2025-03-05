#pragma once


template <typename T>
T wrap(T value, T min, T max) {
  const T range = max - min + 1;
  if (value < min) {
    value += range * ((min - value + range - 1) / range);
  } else if (value > max) {
    value -= range * ((value - max + range - 1) / range);
  }
  return value;
}
