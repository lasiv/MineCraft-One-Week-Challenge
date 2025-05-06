#ifndef NONCOPYABLE_H_INCLUDED
#define NONCOPYABLE_H_INCLUDED

/**
 * @class NonCopyable
 * @brief A base class that disables copy constructor and assignment operator.
 *
 * This class is used to prevent copying of derived classes by deleting the
 * copy constructor and assignment operator. It is useful for classes that
 * should not be copied, such as singletons or resource managers.
 */
struct NonCopyable {
    NonCopyable() = default;
    NonCopyable(const NonCopyable &) = delete;
    NonCopyable &operator=(const NonCopyable &) = delete;
};

#endif // NONCOPYABLE_H_INCLUDED
