# AIDASort
Sort program for the raw file proudced by the AIDA data aquisition system.

## ROOT versions greater than 6.18
If you are using ROOT versions greater than 6.18 you may see errors such as:
```
/root_v6.24.06/include/ROOT/RStringView.hxx:32:37: error: ‘experimental’ in namespace ‘std’ does not name a type
   32 |    using basic_string_view = ::std::experimental::basic_string_view<_CharT,_Traits>;
```
when trying to build. To correct these you need to update the minimum C++ version declared in CMakeLists.txt to C++ 14
```
set_property(TARGET AIDASort PROPERTY CXX_STANDARD 14)
```
