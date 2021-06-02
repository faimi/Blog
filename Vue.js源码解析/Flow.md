
## flow

flow是JavaScript静态类型检查工具，可以让vue拥有静态类型检查的能力，可以减少非常隐蔽的隐患代码

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
数组

```javascript
//数字类型的数组
var arr:Array<number>=[1,2,3]
```

类和对象

```javascript
在这里插入代码片
```
