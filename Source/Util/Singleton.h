#ifndef SINGLETON_H_INCLUDED
#define SINGLETON_H_INCLUDED

#include "NonCopyable.h"
#include "NonMovable.h"

/**
 * @class Singleton
 * @brief A base class for singleton classes.
 * 
 * @details
 * This class is used to create singleton classes by inheriting from it.
 * It combines the functionality of NonMovable and NonCopyable classes
 * to ensure that the singleton instance cannot be moved or copied.
 */
class Singleton : public NonMovable, public NonCopyable {
};

#endif // SINGLETON_H_INCLUDED
