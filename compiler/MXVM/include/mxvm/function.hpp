/**
 * @file function.hpp
 * @brief Dynamic function loader using dlopen/dlsym for runtime symbol resolution
 * @author Jared Bruni
 */
#ifndef __FUNCTION_H__
#define __FUNCTION_H__

#include <dlfcn.h>
#include <stdexcept>

namespace mxvm {

    template <typename Sig>
    class Function;
    /**
     * @brief RAII wrapper for dynamically loaded functions via dlopen/dlsym
     * @tparam R Return type of the loaded function
     * @tparam Args Parameter types of the loaded function
     *
     * Loads a shared library and resolves a function symbol at construction time.
     * The library handle is automatically closed on destruction. Move-only.
     */
    template <typename R, typename... Args>
    class Function<R(Args...)> {
      public:
        using Fn = R (*)(Args...);

        /**
         * @brief Load a function from a named shared library
         * @param lib Path to the shared library
         * @param name Symbol name to resolve
         * @throws std::runtime_error if dlopen or dlsym fails
         */
        Function(char const *lib, char const *name) {
            handle = dlopen(lib, RTLD_LAZY);
            if (!handle)
                throw std::runtime_error(dlerror());
            void *sym = dlsym(handle, name);
            if (!sym) {
                dlclose(handle);
                throw std::runtime_error(dlerror());
            }
            fn = reinterpret_cast<Fn>(sym);
        }

        /**
         * @brief Load a function from the main program's symbol table
         * @param name Symbol name to resolve (via dlopen(nullptr, ...))
         * @throws std::runtime_error if dlsym fails
         */
        Function(char const *name) {
            handle = dlopen(nullptr, RTLD_LAZY);
            if (!handle)
                throw std::runtime_error(dlerror());
            void *sym = dlsym(handle, name);
            if (!sym) {
                dlclose(handle);
                throw std::runtime_error(dlerror());
            }
            fn = reinterpret_cast<Fn>(sym);
        }

        ~Function() {
            if (handle)
                dlclose(handle);
        }

        Function(const Function &) = delete;
        Function &operator=(const Function &) = delete;

        Function(Function &&other) noexcept : handle(other.handle), fn(other.fn) {
            other.handle = nullptr;
            other.fn = nullptr;
        }
        Function &operator=(Function &&other) noexcept {
            if (this != &other) {
                if (handle)
                    dlclose(handle);
                handle = other.handle;
                fn = other.fn;
                other.handle = nullptr;
                other.fn = nullptr;
            }
            return *this;
        }

        /** @brief Invoke the loaded function */
        R operator()(Args... args) const {
            if (!fn)
                throw std::runtime_error("Null function pointer");
            return fn(std::forward<Args>(args)...);
        }

      private:
        void *handle = nullptr;  ///< dlopen library handle
        Fn fn = nullptr;         ///< resolved function pointer
    };

} // namespace mxvm

#endif