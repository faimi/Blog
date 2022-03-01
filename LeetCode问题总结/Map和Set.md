## 229 求众数 II

1、 Map

初始化`let map=new Map()`

（1） map.set(键名，对应值)

当键名是一样的时候，会覆盖对应值

```javascript
let map=new Map();
map.set("name","fmy");
map.set("name","fanmengyu");
console.log(map)  // Map(1) {"name" => "fanmengyu"}
```

（2） map.get(键名)

键名不存在时会返回 undefined

（3） map.has(键名)

键名存在返回 true ，不存在返回 false

**总结： Map 适合用来计算数组里面各个值的个数**

```javascript
let nums = [1, 1, 1, 3, 3, 2, 2, 2];
let len = nums.length;
let more = parseInt(len / 3);
let map = new Map();
for (let i = 0; i < len; i++) {
	let temp = nums[i]
	if (!map.has(temp)) {
		map.set(temp, 1)
	} else {
		map.set(temp, map.get(temp) + 1)
	}
}
console.log(map) //Map(3) {1 => 3, 3 => 2, 2 => 3}
```

## 594 最长和谐子序列

1、 map 的简化写法

```javascript
let nums = [1, 3, 2, 2, 5, 2, 3, 7];
let map = new Map();
nums.forEach(item => map.set(item, (map.get(item) || 0) + 1));
console.log(map) // Map(5) {1 => 1, 3 => 2, 2 => 3, 5 => 1, 7 => 1}
```

2、map.entries()

众所周知 `map forEach()` 第一个参数是值，第二个参数是键名

```javascript
map.forEach((v, k) => {
    console.log(v, k) // 1 1    2 3    3 2    1 5    1 7
});
```

> entries() 方法返回一个新的包含 [key, value] 的 Iterator 对象，返回的迭代器的迭代顺序与 Map 对象的插入顺序相同。

```javascript
[...map.entries()].forEach(([v, k]) => {
    console.log(v, k) // 1 1    3 2    2 3    5 1    7 1
});
```

但是实际上等价于

```javascript
[...map].forEach(([v, k]) => {
    console.log(v, k) // 1 1    3 2    2 3    5 1    7 1
});
```

另外

```javascript
console.log(map.values()); // MapIterator {1, 2, 3, 1, 1}
```

示例：

```javascript
var myMap = new Map();
myMap.set('0', 'foo');
myMap.set(1, 'bar');
myMap.set({}, 'baz');

var mapIter = myMap.entries();

console.log(mapIter);  // MapIterator {'0' => 'foo', 1 => 'bar', {…} => 'baz'}
console.log(mapIter.next());  // {value: Array(2), done: false}
console.log(mapIter.next().value);  // [1, 'bar']
```

## 638 大礼包

1、map没有length但是有size

```javascript
let map = new Map([["name", "fan"]])
console.log(map.length) // undefined
console.log(map.size) // 1
```

## 970 强整数

1、Set集合

当传入相同的值作为参数，那么后续的调用实际上会被忽略

```javascript
let result=[2, 3, 5, 9, 4, 5, 7, 10]; //result.length=8
console.log(new Set(result)) // Set(7) {2, 3, 5, 9, 4, 7, 10}
```

2、Array.from()

将对象转换为数组

```javascript
console.log(Array.from(new Set(result))) //[2, 3, 5, 9, 4, 7, 10]
```

所以Set集合与Array.from()连用就可以帮助数组去重
