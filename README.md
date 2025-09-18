# fixlongoverflow-1.21
修复实体box.minX或者box.maxX坐标取值在[33554412.0,33554436.0)导致游戏IllegalArgumentException崩溃</br>
输出java.lang.IllegalArgumentException: Start element (9223367638808264704) is larger than end element (-9223372036854775808)的bug</br>
</br>
我修的想法是如果这个r是LONG_MAX，那么r+1溢出变成LONG_MIN，所以我直接在起始值大于结束值(l>r)的情况下，</br>
判断r-1是不是LONG_MAX，如果是那么直接替换到tailSet(l)，不过没想到别的注入方式，所以用了很丑陋的@Redirect，大佬们轻喷</br>
</br>
# c++复现案例
[MC_INT64_OVERFLOW_TEST.cpp](MC_INT64_OVERFLOW_TEST.cpp)</br>
**得出方式**
关键问题就是麻将的代码用的subset取值，但是因为subset不包含上边界，</br>
麻将大聪明为了包含上边界直接给值+1L导致如果是LONG_MAX+1L==LONG_MIN，</br>
然后起始值大于结束值触发Java的IAE异常Start element is larger than end element（苦笑）</br>
所以我把关键代码在cpp实现了一遍，包括Java的库函数用到的部分我也抄了</br>
</br>
看这个图片，如果我需要让r是LONG MAX，那么我需要让ChunkSectionPos::asLong(p, -1, -1)返回LONG MAX</br>
<img width="649" height="373" alt="4%MXG7(MNKZNR4 QZ009EWU" src="https://github.com/user-attachments/assets/919da4ad-a361-41f4-8efb-c50109f9466f" /></br>
</br>
查看ChunkSectionPos::asLong代码，发现asLong里面是位运算拼接的，根据实际情况来看，只有第一个p会影响，也就是这里的x，因为ChunkSectionPos::asLong(p, -1, -1)导致y、z全是-1，所以都不影响</br>
<img width="544" height="170" alt="HQEG(X)`WT51HSTXTP_YF28" src="https://github.com/user-attachments/assets/712e72e1-ed10-47e8-ac66-12a55a98464e" /></br>
</br>
而根据他的运算得出x实际上是高63~42位共22bit，所以我们需要构造一个最高位为0其他位全为1的数，正是因为麻将别的y、z都刚好是-1（二进制位全1），</br>
只有x能改变且x刚好是高位，所以4194303 >> 1 = 2,097,151(相当于给这个22bit数的高位置0)之后的值给这个函数最后可以得出一个最高位为0其他位全为1的数</br>
<img width="900" height="354" alt="image" src="https://github.com/user-attachments/assets/d5dccffd-a582-4c01-a131-a07559a25c18" /></br>
<img width="900" height="352" alt="image" src="https://github.com/user-attachments/assets/754246d2-b644-40cf-91e4-10edad15d4db" /></br>
</br>
你看，这不就得到我们想要的值了？现在我们只需要让x是2,097,151就能触发bug了，那么x是怎么来的呢？看这里：</br>
<img width="639" height="430" alt="1 O_G)50D15UJ OV{9MMTXW" src="https://github.com/user-attachments/assets/2104317b-b489-4263-a1c8-ff65051153d0" /></br>
</br>
只要这里的j或者m能得到2,097,151，那么下面的for遍历过程必然触碰到崩溃边界导致溢出问题，所以现在需要知道ChunkSectionPos::getSectionCoord(box.minX - 2.0)或者</br>
ChunkSectionPos::getSectionCoord(box.maxX + 2.0)能得出2,097,151，于是进入函数内部查看，发现是如下情况：</br>
<img width="854" height="142" alt="image" src="https://github.com/user-attachments/assets/5a17780b-d6ad-43c7-9e71-c170a2ede401" /></br>
</br>
getSectionCoord对值调用了Math::floor后再次重载后对值除以16（原始Java代码是>>4），cpp不能保证是无符号或者有符号位移，但是这里和整数乘法效果相当所以直接写整数除法</br>
那既然是对目标值除以16后得到的2,097,151，反过来2,097,151 * 16就能得到Math::floor之后的值为33,554,416：</br>
<img width="900" height="355" alt="image" src="https://github.com/user-attachments/assets/3a998396-d49c-4104-aa57-fdeeaae66afa" /></br>
</br>
查看Math::floor的代码发现其实就是个类型转换，直接舍弃浮点位数，然后在舍弃后原值小于目标值（也就是负数）的情况下对目标值-1保证负数符合预期：</br>
<img width="319" height="86" alt="image" src="https://github.com/user-attachments/assets/c1771132-4a3d-41b9-9570-7f8b4730e4a8" /></br>
</br>
我们目前的值是个正数，所以直接不变，相当于box.minX - 2.0之后就能得到我们的值box.minX = 33,554,416.0 + 2.0 = 33,554,418.0，</br>
因为min到max之间有4的差值，这样就至少可以触发bug，于是编写了一个测试案例：</br>
<img width="346" height="205" alt="image" src="https://github.com/user-attachments/assets/b8e5aec1-e50a-4932-8ed9-734ec0adf9b5" /></br>
</br>
跑一下看结果：</br>
<img width="1049" height="822" alt="image" src="https://github.com/user-attachments/assets/0687c13c-a80b-42de-b5a7-93e4586d8e8c" /></br>


