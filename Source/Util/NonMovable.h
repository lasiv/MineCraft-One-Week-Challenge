#ifndef NON_MOVE
#define NON_MOVE

/**
 * @class NonMovable
 * @brief A base class that disables move constructor and assignment operator.
 * 
 * @details
 * This class is used to prevent moving of derived classes by deleting the
 * move constructor and assignment operator. It is useful for classes that
 * should not be moved, such as singletons or resource managers.
 */
class NonMovable {
  public:
    NonMovable(NonMovable &&) = delete;

    NonMovable &operator=(NonMovable &&) = delete;

  protected:
    NonMovable() = default;
};

#endif // NON_MOVE
