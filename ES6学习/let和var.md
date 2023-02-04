#

##

`a`是个数组，通过 10 次循环，`a`里存放了 10 个函数，所以`a[6]`是取存放在索引 6 的函数`function(){console.log(i)}`

调试的时候是先从第一行代码开始执行

```javascript
var a = [];
for (var i = 0; i < 10; i++) {
  a[i] = function () {
    console.log(i);
  };
}
a[6](); // 10
```

调试的时候直接到 function 里执行

```javascript
var a = [];
for (let i = 0; i < 10; i++) {
  a[i] = function () {
    console.log(i);
  };
}
a[6](); // 6
```

函数内部的变量 i 与循环变量 i 不在同一个作用域，有各自单独的作用域（同一个作用域不可使用 let 重复声明同一个变量）

有两个 Block ，

```javascript
for (let i = 0; i < 3; i++) {
  console.log(i); // 报错ReferenceError
  let i = "abc";
  console.log(i);
}
// abc
// abc
// abc
```

## 暂时性死区

### 原文

只要块级作用域内存在 let 命令，它所声明的变量就“绑定”（binding）这个区域，不再受外部的影响。

ES6 明确规定，如果区块中存在 let 和 const 命令，这个区块对这些命令声明的变量，从一开始就形成了封闭作用域。凡是在声明之前就使用这些变量，就会报错。

总之，在代码块内，使用 let 命令声明变量之前，该变量都是不可用的。这在语法上，称为“暂时性死区”（temporal dead zone，简称 TDZ）。

### 我的话

只要块级作用域内存在 let 命令，它所声明的变量就“绑定”（binding）这个区域，不再受外部的影响。因此在代码块内，使用 let 命令声明变量之前，该变量都是不可用的。并且在没有 let 之前，typeof 运算符是百分之百安全的，永远不会报错。

```javascript
var tmp = 123;
if (true) {
  // TDZ开始
  tmp = "abc"; // ReferenceError
  console.log(tmp); // ReferenceError

  let tmp; // TDZ结束
  console.log(tmp); // undefined

  tmp = 123;
  console.log(tmp); // 123
}
```
