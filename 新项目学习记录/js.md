1. 遍历
用`filter`
2. `find`只能用来数组，返回通过测试（函数内判断）的数组的**第一个元素的值**。
3. `map`生成组件时，组件的`key`不要用`map((item,index) => {})`的`index`，直接用`item`
4. 场景：一直以为通过`temp=[...data]`复制data，就可以对temp修改从而不影响data，但是其实会影响，因为：(es6的扩展运算符(...)的深浅拷贝)[https://www.bbsmax.com/A/MyJxOMoXzn/]