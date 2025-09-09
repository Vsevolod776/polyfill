# C++ Standard Library Polyfill

## Описание

Реализация функций std expected/optional для C++ 11.

### Компоненты

#### 1. `utility.h` - Базовые утилиты
- **`in_place_t`** - тип для in-place конструирования объектов
- **`nullopt_t`** - тип для представления отсутствующего значения в optional
- **`unexpect_t`** - тип для обозначения неожиданного результата в expected

#### 2. `optional.h` - Реализация std::optional
Реализация `std::optional`:
- **Конструкторы**: default, copy/move, value, in-place, от других optional
- **Операторы присваивания**: nullopt, copy/move, value, от других optional
- **Методы доступа**: `value()`, `value_or()`, `has_value()`, операторы `*` и `->`
- **Исключения**: `bad_optional_access` для безопасного доступа к значению
- **Утилиты**: `make_optional()` для создания optional объектов
- **Операторы сравнения**: полный набор операторов сравнения с optional, значениями и nullopt

#### 3. `expected.h` - Реализация std::expected
Реализация `std::expected` из C++23 для обработки ошибок:
- **`Unexpected<E>`** - wrapper для значений ошибок
- **`Expected<T, E>`** - основной тип, содержащий либо значение типа T, либо ошибку типа E
- **Методы доступа**: `value()`, `error()`, `value_or()`, `has_value()`
- **Исключения**: `bad_expected_access` с поддержкой информации об ошибке
- **Конструкторы**: поддержка in-place и unexpect конструирования
- **Операторы сравнения**: сравнение между expected, значениями и unexpected

### Использование

```c
// Использование optional
optional<int> opt = make_optional(42);
if (opt.has_value()) {
    std::cout << opt.value() << std::endl;
}

// Использование expected
Expected<int, std::string> result = 42;
if (result.has_value()) {
    std::cout << result.value() << std::endl;
} else {
    std::cout << "Error: " << result.error() << std::endl;
}
```

--------------------------------------------------------------------------------

Implementation of std expected/optional functions for C++ 11.

### Components

#### 1. `utility.h` - Base Utilities
- **`in_place_t`** - type for in-place object construction
- **`nullopt_t`** - type for representing absent value in optional
- **`unexpect_t`** - type for indicating unexpected result in expected

#### 2. `optional.h` - std::optional Implementation
Implementation of `std::optional` from C++17, including:
- **Constructors**: default, copy/move, value, in-place, from other optionals
- **Assignment operators**: nullopt, copy/move, value, from other optionals
- **Access methods**: `value()`, `value_or()`, `has_value()`, `*` and `->` operators
- **Exceptions**: `bad_optional_access` for safe value access
- **Utilities**: `make_optional()` for creating optional objects
- **Comparison operators**: full set of comparison operators with optionals, values, and nullopt

#### 3. `expected.h` - std::expected Implementation
Implementation of `std::expected` from C++23 for error handling:
- **`Unexpected<E>`** - wrapper for error values
- **`Expected<T, E>`** - main type containing either a value of type T or an error of type E
- **Access methods**: `value()`, `error()`, `value_or()`, `has_value()`
- **Exceptions**: `bad_expected_access` with error information support
- **Constructors**: support for in-place and unexpect construction
- **Comparison operators**: comparison between expected, values, and unexpected

### Usage

```c
// Using optional
optional<int> opt = make_optional(42);
if (opt.has_value()) {
    std::cout << opt.value() << std::endl;
}

// Using expected
Expected<int, std::string> result = 42;
if (result.has_value()) {
    std::cout << result.value() << std::endl;
} else {
    std::cout << "Error: " << result.error() << std::endl;
}
```

---
