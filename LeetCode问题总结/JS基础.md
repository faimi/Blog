## 2 两数相加

1、平方

```javascript
# 10的i次方
Math.pow(10,i)
```

## 4 寻找两个正序数组的中位数

1、取整数部分

方法1：**parseInt()**

```javascript
# 2
parseInt(5/2);
```

方法2：**Math.floor()**

返回小于等于x的最大整数

```javascript
# 1
Math.floor(1.6);
```

2、连接两个数组

方法1：**concat函数**，不会改变现有的数组

```javascript
let a = [1, 2, 3];
let b = [0];

# [1,2,3,0]
console.log(a.concat(b))
```

方法2：**[...arr1,...arr2]**

```javascript
let a = [1, 6];
let b = [0];
let numList = [...a, ...b]
console.log(numList)
```

## 5 替换空格

replace()：
`/ /`是固定写法 
`\s`是转移符号用以匹配任何空白字符，包括空格、制表符、换页符等等
`g`表示全局匹配将替换所有匹配的子串，如果不加`g`当匹配到第一个后就结束了

```javascript
s.replace(/\s/g, '%20');
// 等价于
s.replaceAll(" ", '%20');
```

## 11 盛最多水的容器

1、js使用 sort 函数对复制的数组排序，发现原数组也被排序了

```javascript
let height = [1, 8, 6, 2, 5, 4, 8, 3, 7]
let heightSort = height;
heightSort.sort((a, b) => b - a);
# [8, 8, 7, 6, 5, 4, 3, 2, 1] [8, 8, 7, 6, 5, 4, 3, 2, 1]
console.log(heightSort,height)
```

解决办法：
```javascript
let height = [1, 8, 6, 2, 5, 4, 8, 3, 7]
let heightSort = height.slice(0);
heightSort.sort((a, b) => b - a);
# [8, 8, 7, 6, 5, 4, 3, 2, 1] [1, 8, 6, 2, 5, 4, 8, 3, 7]
console.log(heightSort,height)
```

## 13 罗马数字转整数

1、 在进行 if 判断时，不要先判断不等于，要先判断等于，否则会出错

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

## 14 最长公共前缀

1、关于break退出循环

```javascript
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

```javascript
for i in range(5):
    执行语句
    continue   #结束本次循环，进行下次循环 
```

## 26 删除有序数组中的重复项

最主要的原因在于 splice 会动态更改数组长度，所以一直不对

## 27 移除元素

for循环失败，while会成功

## 28 实现 strStr()

indexOf() 方法可返回某个指定的字符串值在字符串中首次出现的位置。

## 58 最后一个单词的长度

substring 不会改变原字符串

trim() 方法用于删除字符串的头尾空白符。

```javascript
let str = "   fly me   to   the moon  ";
console.log(str.trim());
//fly me   to   the moon
```

split() 方法用于把一个字符串分割成字符串数组。

```javascript
let str="How are you doing today?";
console.log(str.split(" "));
// [How,are,you,doing,today?]
```

