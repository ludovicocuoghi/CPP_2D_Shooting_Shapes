#include <cmath>         // For math functions like sqrt
#include <stdexcept>     // For std::invalid_argument
#include <SFML/System.hpp> // For sf::Vector2

// Template-based 2D vector class
template <typename T>
class Vec2 {
public:
    T x;  // X-coordinate of the vector
    T y;  // Y-coordinate of the vector

    // Default constructor (initializes the vector to (0, 0))
    Vec2() : x(0), y(0) {}

    // Constructor with specified x and y values
    Vec2(T xin, T yin) : x(xin), y(yin) {}

    // Constructor to convert from an sf::Vector2
    Vec2(const sf::Vector2<T>& vec) : x(vec.x), y(vec.y) {}

    // Conversion operator to convert from Vec2 to sf::Vector2
    // Allows Vec2 to be used with SFML functions (e.g., setPosition)
    operator sf::Vector2<T>() {
        return sf::Vector2<T>(x, y);
    }

    // Addition operator (adds two vectors and returns the result)
    Vec2 operator+(const Vec2& rhs) const {
        return Vec2(x + rhs.x, y + rhs.y);
    }

    // Addition assignment operator (adds the given vector to the current one)
    Vec2& operator+=(const Vec2& rhs) {
        x += rhs.x;
        y += rhs.y;
        return *this; // Return the current object by reference
    }
    // Subtraction operator (subtracts two vectors and returns the result)
    Vec2 operator-(const Vec2& rhs) const {
        return Vec2(x - rhs.x, y - rhs.y);
    }

    // Scalar multiplication (scales the vector by a scalar value)
    Vec2 operator*(const T val) const {
        return Vec2(x * val, y * val);
    }

    // Scalar division (divides the vector by a scalar value)
    Vec2 operator/(const T val) const {
        if (val == 0) {
            throw std::invalid_argument("Division by zero!"); // Handle division by zero
        }
        return Vec2(x / val, y / val);
    }

    // Dot product (returns the scalar dot product of two vectors)
    T dot(const Vec2& rhs) const {
        return x * rhs.x + y * rhs.y;
    }

    // Magnitude (returns the length of the vector)
    T magnitude() const {
        return static_cast<T>(std::sqrt(x * x + y * y));
    }

    // Normalize the vector (returns a unit vector with the same direction)
    Vec2 normalize() const {
        T mag = magnitude();
        if (mag == 0) {
            throw std::invalid_argument("Cannot normalize a zero vector!");
        }
        return *this / mag; // Divide by magnitude to normalize
    }

    // Distance (returns the distance between two vectors)
    T distance(const Vec2& rhs) const {
        return (*this - rhs).magnitude(); // Distance is the magnitude of the difference vector
    }

    // Equality operator (compares if two vectors are equal)
    bool operator==(const Vec2& rhs) const {
        return x == rhs.x && y == rhs.y;
    }

    // Inequality operator (compares if two vectors are not equal)
    bool operator!=(const Vec2& rhs) const {
        return !(*this == rhs);
    }

    // Checks if the vector is the zero vector (both x and y are zero)
    bool isZero() const {
        return x == 0 && y == 0;
    }
};

// Typedef for convenience (commonly used with floats)
using Vec2f = Vec2<float>;
