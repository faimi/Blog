
关键词：类型检查

# Flow

Flow 是 JavaScript 静态类型检查工具，可以让 Vue 拥有检查静态类型的能力，可以看[官网链接](https://flow.org/en/docs/getting-started/)系统的学习

## 为什么要用 Flow

因为 JavaScript 是动态类型语言，但是过于灵活会出现过于隐蔽的隐患代码。 JavaScript 现在用 ES6 写，会用 Babel 编译，在编译期没有明显的语法错误是不会报错的，但是在运行阶段会出现 bug 。类型检查是当今动态语言的发展趋势。类型检查就是在编译阶段尽早发现由类型错误引发的 bug 。 **Vue2.0 时已经引入了 Flow 做静态类型检查。** 之所以选择 Flow，主要是因为 Babel 和 ESLint 都有对应的 Flow 插件以支持语法，可以完全沿用现有的构建配置，非常小成本的改动就可以拥有静态类型检查的能力。

## Flow 的工作方式

通常类型检查分成 2 种方式：

1、类型推断：通过变量的上下文来推断出变量类型，然后根据这些推断来检查类型。

2、类型注释（更常用）：事先注释好我们期待的类型，Flow 会基于这些注释来判断。

### 0、 （做测试前）Flow 的安装

```javascript
npm install -g flow-bin
flow init // 创建 .flowconfig 文件
flow // 运行
```

### 1、类型推断

```javascript
function split(str) {
  return str.split(' ')
}

split(11)
```

Flow 检查上述代码后会报错，因为函数 split 期待的参数是字符串，而我们输入了数字。

### 2、类型注释

这段代码是不会报错的。因为在 JavaScript 中 `+` 可以是数字相加也可以是字符相加，也可以是字符串和数字相加。

```javascript
function add(x, y){
  return x + y
}

add('Hello', 11)
```

为了只允许数字和数字相加，需要对 Flow 进行**类型注释**

```javascript
//这个注释表明这个文件是需要 Flow 检查的，否则会跳过这个文件，所以是不能去掉的
/*@flow*/

// 通过在参数后面添加注释的方式
// x: number, y: number-->静态注释
// 返回值： number
function add(x: number, y: number): number {
  return x + y;
}

add("fmy", 22);
```

### 3、Flow 能支持的一些常见的类型注释

1、数组

数组类型注释的格式是 ```Array<T>```，T 表示数组中每项的数据类型

```javascript
var arr:Array<number>=[1,2,3]
arr.push('Hello') // 会报错
arr.push(4) // 不会报错
```

2、类和对象

```javascript
// 类
class Bar {
  x: string;           // x 是字符串
  y: boolean | number;  // y 可以是字符串或者数字
  
  // 构造函数
  constructor(x: string, y: string | number) {
    this.x = x
    this.y = y
  }
}

// new 一个对象
var bar: Bar = new Bar('hello', 4)
// 以下一行代码会产生报错
/*
原因：传入的参数的数量===构造函数定义的参数的数量
解决办法：如果不传某个参数，可以将参数的注释类型改为void（定义的地方和传参的地方要一起改）
*/
var bar: Bar = new Bar('hello')

//普通对象
var obj: { a: string, b: number, c: Array<string>, d: Bar } = {
  a: 'hello',
  b: 11,
  c: ['hello', 'world'],
  d: new Bar('hello', 3)
}
```

3、 Null

若想类型 T 可以为 null 或者 undefined ，只需类似如下写成 ?T 的格式即可。

```javascript
var foo: ?string = null
```

foo 可以为字符串，也可以为 null

## Flow 在 Vue.js 源码中的应用
Vue.js 中有很多自定义的类型， Flow 提出了一个 libdef 的概念可以用来识别这些第三方库或者是自定义类型。

在 Vue 源码中有一个 .flowconfig 文件

```javascript
[libs]
flow
```

flow 对应一个 flow 文件夹，里面的 js 文件中声明了很多自定义类型，例如 vnode.js 文件下 VNodeData 非常复杂。如果遇到某个类型并想了解它完整的数据结构的时候，可以回来翻阅这些数据结构的定义。

```javascript
declare interface VNodeData {
  key?: string | number;
  ....
};
```
