#ifndef UTILS_HPP
#define UTILS_HPP

/**
 * @brief Checks if any given pointer in the argument list is NULL.
 * @param first_ptr First pointer to check.
 * @param first_name String name of the first pointer.
 * @param ... Additional pairs of (void* ptr, char* ptr_name), ending with NULL.
 * @return 0 if all pointers are valid, otherwise -1.
 */
int _check_ptrs(void *first_ptr, const char *first_ptr_name, ...);

#endif // !UTILS_HPP
