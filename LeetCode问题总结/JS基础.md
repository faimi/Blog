## 平方

```javascript
# 10的i次方
Math.pow(10,i)
```

来源：2.两数相加

## 取整数部分

方法1：**parseInt()**

```javascript
parseInt(5/2); //2
```

方法2：**Math.floor()**

返回小于等于x的最大整数

```javascript
Math.floor(1.6); //1
```

来源：4.寻找两个正序数组的中位数

## 返回最大值和最小值

方法1：Math.max 和 Math.min

Math.max：返回传递的参数的最大值。接受的值：整数，浮点数，数字字符串。

```javascript
console.log(Math.max(-1, 4, 6, 12)); // 12
console.log(Math.max(3.14, 43, 1, 0.2)); // 43
console.log(Math.max(0, -13, 4, 654)); // 654
console.log(Math.max("0.65", "-23")); // 0.65
```

Math.min：返回传递的参数的最小值。

```javascript
console.log(Math.min(-1, 4, 6, 12)); // -1
console.log(Math.min(3.14, 43, 1, 0.2)); // 0.2 
console.log(Math.min(0, -13, 4, 654)); // -13
console.log(Math.min("0.65", "-23")); // -23
```

来源：11. 盛最多水的容器

## 关于 if

1、在进行 if 判断时，不要先判断不等于，要先判断等于，否则会出错

```JavaScript
if (s[i + 1] == 'V') {
  romanToInt += 4
  i++
} else if (s[i + 1] == 'X') {
  romanToInt += 9
  i++
} else {
  romanToInt += 1
}
```

来源：13.罗马数字转整数

## 关于 break 和 continue

1、关于break退出循环

```c++
for (int i = 0; i < 10; i++) {
    if (i == 3) {
      break;
    }
    System.out.println(i);
}
结果为：0 1 2
结论：结束for循环
```

2、不要忘记单个的情况

3、continue

```c++
for i in range(5):
    执行语句
    continue   #结束本次循环，进行下次循环 
```

## 42 连续子数组的最大和

贪心算法和动态规划？？？

【要点】加上当前元素只会对最终数组和起减少的作用，而不是增大数组和，所以就直接以当前元素为起点新起数组求最大数组和
