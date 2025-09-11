# fixlongoverflow-1.21
修复实体x坐标取值在33554418.0 ~ 33554433.9之间导致游戏IllegalArgumentException崩溃，</br>
输出java.lang.IllegalArgumentException: Start element (9223367638808264704) is larger than end element (-9223372036854775808)的bug
# c++复现案例
[MC_INT64_OVERFLOW_TEST.cpp](MC_INT64_OVERFLOW_TEST.cpp)
