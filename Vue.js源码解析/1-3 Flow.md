
## flow

flow是JavaScript静态类型检查工具，可以让vue拥有检查静态类型的能力，可以减少非常隐蔽的隐患代码

### 1、flow 的安装

```javascript
npm install -g flow-bin
flow init //创建.flowconfig文件
flow
```

### 2、类型注释

flow 如果没有进行类型注释，会产生一些 bug，为了更准确的运行代码，需要对 flow 进行类型注释

> 如果没有类型注释，在 JavaScript 中，字符串和数字是可以相加的

```javascript
/*这个注释表明这个文件是需要flow检查的，否则会跳过这个文件，所以是不能去掉的*/
/*@flow*/

//x: number, y: number-->静态注释
function add(x: number, y: number): number {
  return x + y;
}

add("fmy", 22);
```

##### 类型注释的类型
1、数组
数组类型注释的格式是 ```Array<T>```，T 表示数组中每项的数据类型

```javascript
var arr:Array<number>=[1,2,3]
```

2、类和对象

```javascript
//类
class Bar {
  x: string;           // x 是字符串
  y: boolean | number;  // y 可以是字符串或者数字
  
  //构造函数
  constructor(x: string, y: string | number) {
    this.x = x
    this.y = y
  }
}
/*
1、传入的参数的数量===构造函数定义的参数的数量
2、如果不传某个参数，可以将参数的注释类型改为void
3、if(构造函数传入的参数是类中定义的) 
    {构造函数传入的参数的注释类型===类中定义的参数的注释类型}
*/
var bar: Bar = new Bar('hello', 4)

//普通对象
var obj: { a: string, b: number, c: Array<string>, d: Bar } = {
  a: 'hello',
  b: 11,
  c: ['hello', 'world'],
  d: new Bar('hello', 3)
}

```

3、Null

```javascript
var foo: ?string = null
```
foo 可以为字符串，也可以为 null