1. 在Ant Design使用`<Table></Table>`时，使用`render`进行渲染，传入的三个参数分别是当前行的值、当前行数据、行索引
```javascript
<Table>
  <Column
    title="操作"
    render={(text, record, index) => ()}
  />
</Table>
```

2. `render`的箭头函数后半部分不能有`{}`，例如`render={(text, record, index) => {}}`

3. `render`如果要用到`index`，一定要写全，不能直接`render={(index) => ()}`

4. 在 react 中不是所有的引用都是要加`{}`的

```javascript
pagination={{
    total: messageDateLength,
    pageSize: 10,
    showSizeChanger: true,
    showQuickJumper: true,
}}
```