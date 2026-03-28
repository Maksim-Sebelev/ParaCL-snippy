# AST serializer

В данном модуле реализована функия

```cpp
void test_generator::serialize(last::AST const & ast, std::filesystem::path const & out);
```

Данная функция генериует в файле out прогамму на языке ParaCL соответсвующую дереву из параметра ast.

Подключение через cmake:

```cmake
add_subdirectory(<path to serializer>)

target_link_libraries(<your target>
  PRIVATE
    ${PARACL_SNIPPY_AST_SERIALIZER_LIB}
)
```
