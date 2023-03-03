## map 存储数组

map 也可以用数组存储，如何 push

```javascript
for (let i = 0; i < strs.length; i++) {
  if (map.get(tempStr[i])) {
    map.get(tempStr[i]).push(strs[i]);
  } else {
    map.set(tempStr[i], [strs[i]]);
  }
}
```

## map 的简化写法

```javascript
let nums = [1, 3, 2, 2, 5, 2, 3, 7];
let map = new Map();
nums.forEach((item) => map.set(item, (map.get(item) || 0) + 1));
console.log(map); // Map(5) {1 => 1, 3 => 2, 2 => 3, 5 => 1, 7 => 1}
```

## 解决 map.forEach() 倒置 v 和 k 的解决方法

众所周知 `map forEach()` 第一个参数是值，第二个参数是键名

```javascript
map.forEach((v, k) => {
  console.log(v, k);
});
```

### 方法 1：[...map.entries()]

> entries() 方法返回一个新的包含 [key, value] 的 Iterator 对象，返回的迭代器的迭代顺序与 Map 对象的插入顺序相同。

```javascript
[...map.entries()].forEach(([k, v]) => {
  console.log(k, v); // 1 1    3 2    2 3    5 1    7 1
});
```

如果不用`forEach`遍历`map`

```javascript
for (const [num, occ] of map.entries()) {
}
```

示例：

```javascript
var myMap = new Map();
myMap.set("0", "foo");
myMap.set(1, "bar");
myMap.set({}, "baz");
console.log(myMap); // Map(3) {"0" => "foo", 1 => "bar", {…} => "baz
var mapIter = myMap.entries();
console.log(mapIter); // MapIterator {'0' => 'foo', 1 => 'bar', {…} => 'baz'}
console.log(mapIter.next()); // {value: ["0", "foo"], done: false}
console.log(mapIter.next().value); // [1, 'bar']
```

### 方法 2: [...map]

```javascript
[...map].forEach(([k, v]) => {
  console.log(k, v); // 1 1    3 2    2 3    5 1    7 1
});
```

## map 没有 length 但是有 size

```javascript
let map = new Map([["name", "fan"]]);
console.log(map.length); // undefined
console.log(map.size); // 1
```

## Set 集合去重

当传入相同的值作为参数，那么后续的调用实际上会被忽略

```javascript
let result = [2, 3, 5, 9, 4, 5, 7, 10]; //result.length=8
console.log(new Set(result)); // Set(7) {2, 3, 5, 9, 4, 7, 10}
```

```javascript
console.log(Array.from(new Set(result))); //[2, 3, 5, 9, 4, 7, 10]
```

map.values()：返回 Map 对象的所有值
Array.from()：将对象转换为数组

```javascript
Array.from(map.values()); // [["eat", "tea", "ate"], ["tan", "nat"], ["bat"]]
```

所以 Set 集合与 Array.from()连用就可以帮助数组去重

## forEach 不能用 break 停止

```javascript
for (const [k, v] of wordmap) {
    if(){
        break
    }
}
```

## 涉及到`Map`，用`const .. of ..`和`map.forEach`最优

## 数组化 map 的 keys 和 values

```javascript
Array.from(map.keys());
Array.from(map.values())
```